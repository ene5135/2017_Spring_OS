# Project 03

# Weighted Round-Robin Scheduler

# Introduction
The objective of this project is to implement new scheduler policy, weighted round-robin. Also we have to adjust it on our ARTIK device's kernel.

# Implementation
## 1. `sched_wrr_entity` definition
We made definition of `sched_wrr_entity` in `include/linux/sched.h`, and added it in `struct task_struct`.

```c
struct sched_wrr_entity {
	struct list_head run_list;
	unsigned int weight; 	 /* default weight = 10, [1,20] */
	unsigned int time_slice; /* default time slice = 10 * 10ms = 100ms */
	unsigned int tick_left;  /* left # of tick count */
	unsigned int movable;	 /* if(not running && in queue) 1 else 0 */
};
```
`run_list` consists of another sched_wrr_entities.
`weight` is the value which determines `time_slice` of the task. `weight`should be in [1,20] range.
`time_slice` is the value which indicates the length of time which the task can hold the processor in it's turn.
`tick_left` is the value which indicates the left count of cpu tick.
`movable` indicates that the task is movable to another running queue or not. If the task isn't running and in queue, the value is 1. Else the value is 0.


## 2. `wrr_rq` definition
We made definition of `wrr_rq` in `kernel/sched/sched.h`, and added it in `rq`.

```c
struct wrr_rq {
	struct list_head queue_head;
	unsigned long sum_weight;
};
```
`queue_head` is head of the queue which consists of tasks in wrr run queue.
`sum_weight` is sum of all tasks' weight which are in the queue.

## 3. `wrr_sched_class` implementation
To implement new scheduling policy, we have to make sure that all functions of struct sched_class are appropriately implemented. There are 20 functions in sched_class, and we implemented 10 of them in our `wrr_sched_class` and left another 10 by skeleton.


* `enqueue_task_wrr` decides the task should be enqueued or not. And if it should be, then hang the task's sched_wrr_entity at `queue_head`'s tail which is in `wrr_rq`.
* `dequeue_task_wrr` deletes the task's `sched_wrr_entity` from `wrr_rq`.
* `yield_task_wrr` moves the first task in queue to the tail.
* `pick_next_task_wrr` picks the next task in queue which is at the head of the queue.
* `put_prev_task_wrr` makes the current task pause and modify it's `movable` by 1.
* `select_task_rq_wrr` selects a cpu which has the lowest sum of weight.
* `set_cpus_allowed_wrr` returns # of cpus which the task is allowed to move in.
* `set_curr_task_wrr` modify current task's `movable` value by 0, which means it's not movable.
* `task_tick_wrr` calculates the remain # of `tick_left` and decides the task to reschedule or not.
* `get_rr_interval` transforms time_slice into initial `tick_left` value.

With these functionalities above, we've made definition of `wrr_sched_class`. We modified `next` to `fair_sched_class`, because fair policy run queue have less priority than wrr run queue.

```c
const struct sched_class wrr_sched_class = {
	.next		= &fair_sched_class,
	.enqueue_task	= enqueue_task_wrr,
	.dequeue_task	= dequeue_task_wrr,
	...
};
```
## 4. Load Balancing
### 4-1. hrtimer
It is struct that we use to call load_balance every 2 seconds.
in entire system, only one timer(struct hrtimer) count system_time.  
In core.c, init_wrr_hrtimer is called in sched_init and start_wrr_hrtimer is called in sched_init_smp. so timer start. ktime_t(how long period we want) is made 2 seconds. in every 2 seconds, call-back-function(call_load_balance_wrr) is called. 
### 4-2. `load_balance_wrr` implementation
load_balance_wrr is called by hrtimer every 2 seconds.
first, We have to find max_rq(source cpu), min_rq(destination cpu).
and, We have to find heaviest among task_struct that can go to min_cpu(see cpumask, movable...) from max_cpu. and this task doesn't cause the weight imbalance to reverse.
and move this task

## 5. get_weight(), set_weight()
we use find_process_by_pid, task_rq_lock, check_smae_owner in sched_setweight. those function is static in core.c. so we implement sched_setweight, sched_getweight in core.c. and system calls are implemented in sched.c
sched_getweight is protected by rec_read_lock.
sched_setweight is protected by task_rq_lock.

# Improve
We use age-concept. if age is increased, weight++(1<= weight <= 20).
so long-time process's weight is increased until 20.

# Lessons Learned

# Thank you.
