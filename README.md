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
	unsigned int tick_left;  // left # of tick count
	unsigned int movable;	// if(not running && in queue) -> 1
					// else -> 0
};
```
`run_list` consists of another sched_wrr_entities.
`weight` is the value which determines `time_slice` of the task. `weight`should be in [1,20] range.
`time_slice` is the value which indicates the length of time which the task can hold the processor in it's turn.
`tick_left` is the value which indicates the left count of cpu tick.
`movable` indicates that the task is movable to another running queue or not. If the task isn't running and in queue, the value is 1. Else the value is 0.

`sched_wrr_entity`

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
### 3-1. enqueue_task_wrr, dequeue_task_wrr, yield_task_wrr
### 3-2. pick_next_task_wrr
### 3-3. 

	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,
	.yield_task		= yield_task_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,
	.select_task_rq		= select_task_rq_wrr,
	.set_cpus_allowed	= set_cpus_allowed_wrr,
	.set_curr_task		= set_curr_task_wrr,
	.task_tick		= task_tick_wrr,
	.get_rr_interval	= get_rr_interval_wrr,
	

## 4. Load Balancing
### 4-1. hrtimer
### 4-2. `load_balance_wrr` implementation

## 5. get_weight(), set_weight()

# Improve

# Lessons Learned

# Thank you.
