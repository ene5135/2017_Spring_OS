#include <linux/sched.h>
#include <linux/slab.h>

#include "sched.h"

/////// shinhwi ///////

/////// check Quantum, weight///////
#define QUANTUM 10
#define DEFUALT_WEIGHT 10
#define MIN_WEIGHT	1
#define MAX_WEIGHT	20
#define TICK_FACTOR	1/HZ

// 1(MIN_WEIGHT) <= valid weight <= 20(MAX_WEIGHT)

static int valid_weight(unsigned int weight)
{
	if (weight >= SCHED_WRR_MIN_WEIGHT && weight <= SCHED_WRR_MAX_WEIGHT)
		return 1;
	else
		return 0;
}

// Initialize task_struct p
static void init_task_wrr(struct task_struct *p){
	
	if (p == NULL){
		return;
	}

	struct sched_wrr_entity *wrr_se =&(p->wrr);
	// first initializing make weight 0
	// so it is not valid
	// but if weight already exist (migration)
	// I use weigth already existed
	if(!valid_weight(wrr_se->weight)){
		wrr_se->weight = DEFUALT_WEIGHT;
	}

	wrr_se->time_slice = wrr_se->weigth * QUANTUM;
	wrr_se->time_left = time_slice / TICK_FACTOR;
	INIT_LIST_HEAD(&wrr_se->run_list);
}

static struct task_struct *get_task_from_wrr_se(struct sched_wrr_entity *wrr_se){
	return container_of(wrr_se, struct task_struct, wrr);
}

static void update_curr_wrr(struct rq *rq){
	
	struct task_struct *curr = rq->curr;
	struct sched_wrr_entity *wrr_se = &(curr->rt);
	u64 delta_exec;

	if(curr->sched_class != &wrr_sched_class)
		return;

	delta_exec = rq->clock_task - curr->se.exec_start;
	if(unlikely((s64)delta_exec <= 0))
		return;

	schedstat_set(curr->se.statistics.exec_max,
			  max(curr->se.statistics.exec_max, delta_exec));

	curr->se.sum_exec_runtime += delta_exec;
	account_group_exec_runtime(curr, delta_exec);

	curr->se.exec_start = rq->clock_task;
	cpuacct_charge(curr, delta_exec);
	
	// shinhwi think avg_update is not needed
	// shinhwi think bandwidth is not needed
	// shinhwi think wrr_rq -> run_time is not needed


}

// enqueue_Task
static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	// no idea about flags
	struct wrr_rq *wrr_rq = &(rq -> wrr);
	struct sched_wrr_entity *new_wrr_se  = &(p-> wrr);

	// Initialize
	init_task_wrr(p);

	// check whether it is needed?
	if (!list_empty(&(new_wrr_se->run_list))){
		return;
	}
	// add into tail of wrr_rq
	list_add_tail( &(wrr_se->run_list), &(target_wrr_rq->queue_head));
	// increase sum_weight
	target_wrr_rq->sum_weight += wrr_se->weight;
	// make movable = 1
	if (!task_current(rq, p) && p->nr_cpus_allowed >1){
		new_wrr_se->movable = 1;
	}
	// increase nr_running of rq
	inc_nr_running(rq);
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){

	struct wrr_rq *wrr_rq = &(rq-> wrr);
	struct sched_wrr_entity *wrr_se = &(p->wrr);

	update_curr_wrr(rq);
	// delete from wrr_rq
	
	list_del(&(wrr_se->run_list));

	// decrease sum_weight
	wrr_rq->sum_weight -= wrr_se->weight;
	
	wrr_se->movable = 0;

	// dereasse nr_running of rq
	dec_nr_running(rq);
}

static void yield_task_wrr(struct rq *rq){
	
	struct wrr_rq *target_wrr_rq = &(rq->wrr);
	struct list_head *curr_wrr_se = &((rq->curr)->wrr);
	struct list_head *curr_run_list =  &(((rq->curr)->wrr).run_list);
	
	// move first to tail
	list_move_tail(curr_run_list, &(target_wrr_rq->queue_head));
	
}

static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags){
}


static struct task_struct *pick_next_task_wrr(struct rq *rq){
	
	struct wrr_rq *target_wrr_rq = &(rq->wrr);
	
	//if empty 
	if(list_empty(&(target_wrr_rq->queue_head))){
			return NULL;
	}
	//  maybe....... next task is first of queue
	struct list_head *next_run_list = (target_wrr_rq->queue_head).next;
	struct sched_wrr_entity *next_wrr_se 
		= container_of(next_run_list, struct sched_wrr_entity, run_list);
	struct task_struct *next_task = get_task_from_wrr_se(next_wrr_se);
	
	(next_task->se).exec_start = rq->clock_task;
	
	//make next_task non-movable
	next_wrr_se->movable = 0;
	
	return next_task;

}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p) {
	
	struct sched_wrr_entity *p_wrr_se = &(p->wrr);
	update_curr_wrr(rq);

	if(!list_empty(&(p_wrr_se->run_list)) && p->nr_cpus_allowed > 1){
		wrr_se->movable = 1;
	}
}

//select_task_rq_wrr

//rq_online_wrr


static void set_curr_task_wrr(struct rq *rq){
	struct task_struct *p = rq->curr;

	p->se.exec_start = rq->clock_task;
	(p->wrr).movable = 0;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued){
	struct sched_wrr_entity *wrr_se = &(p->wrr);

	update_curr_wrr(rq);

	// p is not wrr_policy 
	if(p->policy != SCHED_WRR){
		return;
	}
	// time_left is not 0
	if(--wrr_se->time_left){
		return;
	}

	// time_left is 0
	// we have to reschedule
	wrr_se -> time_left = wrr_se -> time_slice / TICK_FACTOR;
	
	// task_struct is absolutely completed
	if(wrr_se->run_list.prev == wrr_entity->run_list.next){
		set_tsk_need_resched(p);
	}
	else{// task_struct is not completed, but time_left is not left
		list_move_tail(&(wrr_se->run_list), &(rq->wrr.queue_head));
		set_tsk_need_resched(p);
	}
	
}


// be called by the get_interval system call
// sched_rr_get_interval 
// core.c -> SYSCALL_DEFINE2
// get_rr_interval_wrr - return the default timeslice of a process
static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task){
	unsigned int time_slice = task->wrr.time_slice;
	return time_slice / TICK_FACTOR;
}

static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio){
	
	// We has no notion of priority
	// so Do not need to implement.
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p){
	
	// I think this method shoud not need.
}

static void init_wrr_rq(struct wrr_rq *wrr_rq)
{
  INIT_LIST_HEAD(&wrr_rq->queue_head);
  wrr_rq->sum_weight = 0;
}
  

static const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task	= enqueue_task_wrr,
	.dequeue_task	= dequeue_task_wrr,
	.yield_task		= yield_task_wrr,

	.check_preempt_curr	= check_preempt_curr_wrr,

	.pick_next_task	= pick_next_task_wrr,
	.put_prev_task	= put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq	= select_task_rq_wrr,
	.rq_online		= rq_online_wrr,
	.rq_offline		= rq_offline_wrr,
	.pre_schedule	= pre_schedule_wrr,
	.post_schedule	= post_schedule_wrr,
	.task_woken		= task_woken_wrr,
	.switched_from	= switched_from_wrr,
#endif

	.set_curr_task	= set_curr_task_wrr,
	.task_tick		= task_tick_wrr,

	.get_rr_interval= get_rr_interval_wrr,

	.prio_changed	= prio_changed_wrr,
	.switched_to	= switched_to_wrr,
};