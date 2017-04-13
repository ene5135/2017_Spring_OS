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
 
 ## 5. Lock acquiring policy
 
 ## 6. Error handling
 
 # Lessons Learned
 
 # Thank you.
