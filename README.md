# Project 03

# Weighted Round-Robin Scheduler

# Introduction
The objective of this project is to implement new scheduler policy, weighted round robin.

# Implementation
## 1. sched_wrr_entity
~/include/linux/sched.h

struct sched_wrr_entity {
	struct list_head run_list;
	unsigned int weight; /* default = 10, [1,20] */
	unsigned int time_slice; /* base time slice(quantum = 10ms) */
							 // so default time slice = 100ms
	unsigned int tick_left;  // left # of tick count
	unsigned int movable;	// if(not running && in queue) -> 1
					// else -> 0
};
## 2. wrr_rq
~/kernel/sched/sched.h

struct rq {
  ...
  struct wrr_rq wrr;
  ...
};

struct wrr_rq {
	struct list_head queue_head;
	unsigned long sum_weight;
};

## 3. wrr_sched_class
### 3-1. enqueue_task_wrr, dequeue_task_wrr, yield_task_wrr
### 3-2. pick_next_task_wrr
### 3-3 

## 4. Load Balancing

## 5. get_weight(), set_weight()

# Lessons Learned

# Thank you.
