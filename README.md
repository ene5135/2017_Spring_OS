# Project 02

# Rotation Lock

# Introduction
This assignment includes five system calls in `kernel/rotation.c`. `set_rotation()` set current rotation of the artik device and maintain the rotation data by a global variable `global_rotation`. `rotlock_read()` and `rotlock_write()` decides the caller to sleep or not, according to rotation status, list of acquired processes and waiting processes. And also the system call make the caller process to sleep if it's required. `rotunlock_read()` and `rotunlock_write()` removes the lock acquired information from the list.
 
# Implementation
## 1. Device's rotation data
Unfortunately, ARTIK device doesn't have a sensor related to rotation data. So we used `rotd.c` which is offered by OS TAs, to update virtual rotation data every 2 seconds. So we implemented `set_rotation()` which `rotd.c` will call to update the rotation value. `global_rotation` is a global variable which maintains the rotation value. `global_lock` is a global lock variable which will be used in system call implementations below, to protect several data structure we must maintain.
  
```c
  int global_rotation = 0;
  DEFINE_SPINLOCK(global_lock);
```

`global_rotation`is updated in `set_rotation()`.
  
```c
  asmlinkage long sys_set_rotation(int degree)
  {
   ...
   
   spin_lock(&global_lock);
   
   ...
   
   global_rotation = degree;
   
   ...
   
   spin_unlock(&global_lock);
   
   ...
  }
```

Since `global_rotation` can be accessed by multiple processes, we locked it to protect it.
   
## 2. Process lock information
Since we have to know which process acquired or is waiting for certain rotation lock, we implemented a data struct in `include/linux/rotation.h`. The name of struct is `rot_lock_info`. The description is below.

```c
struct proc_lock_info
{
	struct task_struct * task;
	int range;
	int degree;
	int type;
	struct list_head sibling;
};
```

`task_struct` of the process is included because of several reasons. We will cover about this later.
`list_head` is included to make list of `proc_lock_info`.

`rotation.c` has two list of `proc_lock_info`, which are list of waiting processes and acquired processes. Initialization is below.

```c
LIST_HEAD(waiting_list_head);
LIST_HEAD(acquiring_list_head);
```

## 3. Make sleep
  If `sys_rotlock_read()` or `sys_rotlock_write()` noticed that the caller process cannot acquire the lock yet, the caller process should sleep. To make certain process sleep, there are some steps to be done. First, the process' state should be set to interruptible. Second, the function `schedule()` should be called to let the scheduler notice the caller process' state is changed, and so the process should take a sleep. Then the scheduler will make the process sleep. Brief description of the steps is below.
  
```c
  asmlinkage long sys_rotlock_read(int degree, int range)
  {
   spin_lock(&global_lock);
   
   ...
   
   if (certain condition)
   {
    set_current_state(TASK_INTERRUPTIBLE);
    
    ...
   }
   
   spin_unlock(&global_lock);
   
   if (current state is TASK_INTERRUPTIBLE)
   {
    schedule();
    ...
```
`global_lock` should be grabbed because the list of acquired/waiting processes will be modified. `global_lock` should not be released before the process state is updated to interruptible. And also, `global_lock` should be released before `schedule()`is called. Otherwise, the caller process will grab the `global_lock` and fall in sleep, which will cause the deadlock.
  
## 4. Waking up
Waking up is pretty more complicated compare to make sleep. While make sleep happens in `sys_rotlock_read()` and `sys_rotlock_write()` directly, waking up can happen in various conditions. We classified the conditions in three cases. 

   1. When certain acquired lock is unlocked. 
   2. When the device's rotation values is changed by `set_rotation()`. 
   3. When the process, which was grabbing a lock, is suddenly terminated.
   
So we decided to implement a common function to cover various waking up cases. The function is called when a lock is released, the device rotation is changed or certain process which was grabbing the lock is terminated, as we mentioned right before. Then, the function actually wakes up the process which is chosen by our lock acquiring policy. We named the function `rescheduler()`.
   
### 4-1. `rescheduler()` implementation.
`rescheduler()` iterates the waiting process' list. And if certain waiting process is confirmed that it's okay to get the lock, then `rescheduler()` deletes process lock information from the list and calls `wake_up_process()`. Brief description is below.
   
```c
     void rescheduler(void)
     {
     	...
	
     	list_for_each_entry_safe(cursor, temp, &waiting_list_head, sibling)
	{
		if (condition)
		{
			...
			wake_up_process(cursor->task);
			...
		}
		...
	}
	...
```
    
### 4-2. Where `rescheduler()` is called
We will show you brief descriptions that where `rescheduler()` is called. First case is in `set_rotation()`.
```c
      asmlinkage long sys_set_rotation(int degree)
      {
      	...
	spin_lock(&global_lock);
	...
	/* modify the global rotation value */
	...
	
	rescheduler();
	
	spin_unlock(&global_lock);
	...
       }
```
Second case is in `rotunlock_read()` and `rotunlock_write()`.
      
```c
      asmlinkage long sys_rotunlock_read(int degree, int range)
      {
      	...
	spin_lock(&global_lock);
	
	list_for_each_entry_safe(cursor, temp, &acquiring_list_head, sibling)
	{
		...
		/* delete and free the process info entry */
		...
	}
	
	rescheduler();
	
	spin_unlock(&global_lock);
	...
      }
```
Third case is when the lock acquired process is terminated. We implemented `exit_rotlock()` which updates the process lock information list and calls the `rescheduler()`.
	
```c
	void exit_rotlock(void)
	{
		...
		spin_lock(&global_lock);
		
		list_for_each_entry_safe(cursor, temp, &acquiring_list_head, sibling)
		{
			if (condition)
			{
				...
				rescheduler();
				spin_unlock(&global_lock);
				return;
			}
		}
		...
		/* repeat same instructions for &waiting_list_head */
		...
	}
```
And, we should call `exit_rotlock()` when every termination of every process. So we decided to put `exit_rotlock();` in `do_exit()` in `kernel/exit.c`.
	
```c
	/* do_exit() - the system call which always called at every terminations of processses */
	void do_exit(long code)
	{
		...
		exit_rotlock();
		...
	}
```
   
## 5. Lock acquiring policy
There are five basic rules of lock acquiring policy.
   
  1. Read lock can be grabbed by multiple processes.
  2. Write lock can only be grabbed by a single process.
  3. Read lock and write lock cannot be grabbed simultaneously.
  4. If read lock is acquired and write lock is waiting, no more read lock can be acquired(Write lock starvation prevention).
  5. rules 1~4 all are applied for only locks which cover the current rotation in their range.

rules 1~4 are represented by function `check_acquiring_list()`. And rule 5 is represented by function `is_overwrapped()` and `is_in_range()`. In `rotlock_read()` and `rotlock_write()`, both system calls call `is_in_range()` to check if the current rotation of device is in the caller process' range argument. And then, `check_acquiring_list()` looks for lock acquired process list to follow the rules 1~4. `check_waiting_write()` represents rule 4. It returns if there is write lock waiting or not. Brief logistic flow of `check_acquiring_list()` is below.

```c
int check_acquiring_list(struct proc_lock_info * new_proc)
{
	...
	/* checks for processes which are acquiring lock */
	list_for_each_entry(cursor, &acquiring_list_head, sibling)
	{
		/* checks for the cursor process' lock covers the current rotation(rule 5) */
		if (is_overwrapped(cursor->degree, cursor->range, new_proc->degree, new_proc->range)
		{
			/* if cursor process lock is write lock, new_proc cannot acquire the lock(rule 2,3) */
			if (is_writer(cursor)) return 0;
			/* else if cursor process lock is read lock, */
			else {
				/* if new_proc wants write lock, the lock cannot be acquired(rule 3)
				if (is_writer(new_proc))
					return 0;
				/* else, when read lock is acquired and new_proc wants read lock, we need to check if there is a process which waits for write lock(rule 4) */
				else if (check_waiting_write())
					return 0;
			}
		}
	}
	/* if there is no overwrapped locks or only read locks are acquired and no writing lock waiting, lock can be grabbed */
	return 1;
}
```


`check_acquiring_list()` is called not only in read/write lock functions but also `rescheduler()` to decide which waiting process can be woken up in certain situation.

```c
 asmlinkage long sys_rotlock_read(int degree, int range)
 {
 	...
	if (is_in_range(degree, range, global_rotation) && check_acquiring_list(new_proc))
	{
		/* can acquire the lock */
		...
	}
	else 
	{
		/* cannot acquire the lock. the process should wait. */
		...
	}
	...
}
```

```c
void rescheduler(void)
{
	...
	list_for_each_entry_safe(cursor, remp, &waiting_list_head, sibling)
	{
		if (is_in_range(cursor) && check_acquiring_list(cursor))
		{
			/* The process can get the lock. Wake it up! */
			...
		}
	}
	...
}
```
			
   
   
 
 ## 6. Exception handling
 
 # Lessons Learned
 
 # Thank you.
