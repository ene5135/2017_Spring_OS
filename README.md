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
We made definition of `wrr_rq` in `kernel/sched/sched.h`, and added it in `struct rq`.

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
	

## 4. Load Balancing
### 4-1. hrtimer
### 4-2. `load_balance_wrr` implementation

## 5. get_weight(), set_weight()

# Improve

# Lessons Learned

# Thank you.
