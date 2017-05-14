#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/cpumask.h>

#include "sched.h"

/////// shinhwi ///////

/////// check Quantum, weight///////
// 500 tick
#define QUANTUM 50
#define DEFUALT_WEIGHT 10
#define	SCHED_WRR_MIN_WEIGHT	1
#define SCHED_WRR_MAX_WEIGHT	20
#define TICK_FACTOR	HZ/1000/*1/HZ*/

// 1(MIN_WEIGHT) <= valid weight <= 20(MAX_WEIGHT)


struct hrtimer wrr_hrtimer;

enum hrtimer_restart test(struct hrtimer *timer)
{
	ktime_t period = ns_to_ktime(2000*1000000);
	printk(KERN_ERR "hello\n");

	hrtimer_forward(timer, timer->base->get_time(), period);

  	return HRTIMER_RESTART;
}

void init_wrr_hrtimer(void)
{

	printk(KERN_ERR "hello_init\n");
	hrtimer_init( &wrr_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
  	wrr_hrtimer.function = test;

}

void start_wrr_hrtimer(void)
{

	printk(KERN_ERR "hello_start\n");
	int delay_in_ms = 2000;
	ktime_t ktime = ns_to_ktime(delay_in_ms*1000000);

  	hrtimer_start( &wrr_hrtimer, ktime, HRTIMER_MODE_REL );
}


// 1(MIN_WEIGHT) <= valid weight <= 20(MAX_WEIGHT)
int valid_weight(unsigned int weight)
{
	if (weight >= SCHED_WRR_MIN_WEIGHT && weight <= SCHED_WRR_MAX_WEIGHT)
		return 1;
	else
		return 0;
}

// Initialize task_struct p
static void init_task_wrr(struct task_struct *p){
	
	struct sched_wrr_entity *wrr_se;
	
	if (p == NULL){
		return;
	}
	
	wrr_se = &(p->wrr);
	// first initializing make weight 0
	// so it is not valid
	// but if weight already exist (migration)
	// I use weight already existed
	if (!valid_weight(wrr_se->weight)) {
		wrr_se->weight = DEFUALT_WEIGHT;
	}

	wrr_se->time_slice = wrr_se->weight * QUANTUM;
	wrr_se->tick_left = /*100;*/wrr_se->time_slice * TICK_FACTOR;
	INIT_LIST_HEAD(&wrr_se->run_list);
}

static struct task_struct *get_task_from_wrr_se(struct sched_wrr_entity *wrr_se){
	return container_of(wrr_se, struct task_struct, wrr);
}

static void update_curr_wrr(struct rq *rq){
	
	struct task_struct *curr = rq->curr;
	// struct sched_wrr_entity *wrr_se = &(curr->wrr);
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
	list_add_tail( &(new_wrr_se->run_list), &(wrr_rq->queue_head));
	// increase sum_weight
	wrr_rq->sum_weight += new_wrr_se->weight;
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
	struct sched_wrr_entity *curr_wrr_se = &((rq->curr)->wrr);
	struct list_head *curr_run_list = &(curr_wrr_se->run_list);
	// struct list_head *curr_run_list = &(((rq->curr).wrr).run_list);
	
	// move first to tail
	list_move_tail(curr_run_list, &(target_wrr_rq->queue_head));
	
}

static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags){
}


static struct task_struct *pick_next_task_wrr(struct rq *rq){
	
	struct wrr_rq *target_wrr_rq = &(rq->wrr);
	
	//  maybe....... next task is first of queue
	struct list_head *next_run_list;
	struct sched_wrr_entity *next_wrr_se; 
	struct task_struct *next_task;
	
	//if empty 
	if (list_empty(&(target_wrr_rq->queue_head))){
			return NULL;
	}
	
	next_run_list = (target_wrr_rq->queue_head).next;
	next_wrr_se = container_of(next_run_list, struct sched_wrr_entity, run_list);
	next_task = get_task_from_wrr_se(next_wrr_se);
	
	(next_task->se).exec_start = rq->clock_task;
	
	//make next_task non-movable
	next_wrr_se->movable = 0;
	
	return next_task;
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *p) {
	
	struct sched_wrr_entity *p_wrr_se = &(p->wrr);
	update_curr_wrr(rq);

	if (!list_empty(&(p_wrr_se->run_list)) && p->nr_cpus_allowed > 1){
		p_wrr_se->movable = 1;
	}
}

static void set_curr_task_wrr(struct rq *rq){
	struct task_struct *p = rq->curr;

	p->se.exec_start = rq->clock_task;
	(p->wrr).movable = 0;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued){
	struct sched_wrr_entity *wrr_se = &(p->wrr);

	update_curr_wrr(rq);
	
	//if(p->pid > 4000)
	//	printk(KERN_ERR "task_tick_wrr : pid %d, policy %u, tick_left %u HZ %d\n",p->pid,p->policy,wrr_se->tick_left,HZ);

	// p is not wrr_policy 
	if(p->policy != SCHED_WRR){
		return;
	}
	// tick_left is not 0
	if(--wrr_se->tick_left){
		return;
	}

	// tick_left is 0
	// we have to reschedule
	wrr_se -> tick_left = /*100;*/wrr_se -> time_slice * TICK_FACTOR;
	
	// task_struct is absolutely completed
	if(wrr_se->run_list.prev == wrr_se->run_list.next){
		set_tsk_need_resched(p);
	}
	else{// task_struct is not completed, but tick_left is not left
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
	
	// We have no notion of priority
	// so no need to implement.
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p){
	
	// I think we don't need this method.
}

static void set_cpus_allowed_wrr(struct task_struct *p, const struct cpumask *new_mask){
	struct rq *rq;
	int weight;

	weight = cpumask_weight(new_mask);

	if(( p->nr_cpus_allowed > 1) == (weight > 1))
		return;

	rq = task_rq(p);

	if(weight <= 1){
		if(!task_current(rq, p)){
			movable = 0;
		}
	}
	else {
		if(!task_current(rq, p)){
			movable = 1;
		}
	}
}
static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags){
	
	struct task_struct *curr;
	struct rq *rq;
	int cpu, lowest_cpu = -1, lowest_weight = -1;
	int cpu_arr;
	unsigned long cpu_bit;

	rcu_read_lock();

	cpu = task_cpu(p);

	if (p->nr_cpus_allowed == 1)
		goto out;
	
	//if ( sd_flag != SD_BALANCE_WAKE && sd_flag != SD_BALANCE_FORK )
	//	goto out;

	
	//find smallest rq of online cpu
	for_each_online_cpu(cpu) {
		cpu_arr = cpu / (sizeof(unsigned long) * 8);
		cpu_bit = 1 << (cpu % (sizeof(unsigned long) * 8));

		if (!(cpu_bit & (p->cpus_allowed.bits[cpu_arr])))
			continue;

		rq = cpu_rq(cpu);

		if (lowest_weight == -1 || lowest_weight > rq->sum_weight) {
			lowest_weight = rq->sum_weight;
			lowest_cpu = cpu;
		}
	}
out:
	rcu_read_unlock();

	if (lowest_cpu != -1)
		return lowest_cpu;
	else 
		return cpu;
}

static void rq_online_wrr(struct rq *rq){
	// called when a cpu goes online.
	__enable_runtime(rq);
}
static void rq_offline_wrr(struct rq *rq){
	// called when a cpu goes offline.
	__disable_runtime(rq);
}
static void pre_schedule_wrr(struct rq *rq, struct task_struct *prev){
}
static void post_schedule_wrr(struct rq *rq){
}
static void task_woken_wrr(struct rq *rq, struct task_struct *p){
}
static void switched_from_wrr(struct rq *rq, struct task_struct *p){
}


void init_wrr_rq(struct wrr_rq *wrr_rq)
{
  INIT_LIST_HEAD(&wrr_rq->queue_head);
  wrr_rq->sum_weight = 0;
}

void move_task(int max_cpu, int min_cpu) {

	struct rq *max_rq = cpu_rq(max_cpu);
	struct rq *min_rq = cpu_rq(min_cpu);
	struct wrr_rq *max_wrr_rq = max_rq->wrr;
	struct wrr_rq *min_wrr_rq = min_rq->wrr;
	int max_sum = max_wrr_rq->sum_weight;
	int min_sum = min_wrr_rq->sum_weight;
	
	struct task_struct *p, *move_p;
	int max_weight = -1;

	double_rq_lock(max_rq, min_rq);
	
	if (max_cpu == min_cpu)
		return;

	/* choose a task to migrate */
	plist_for_each_entry(p, max_wrr_rq->queue_head, queue_head) {
		/* when the task is not movable */
		if (!(p->wrr).movable)
			continue;
		/* when the task is not allowed to run on min_cpu */
		if (!cpumask_test_cpu(min_cpu, &(p->cpus_allowed))
			continue;
		/* when the sum_weight order seems to be changed after the migration */
		if ((se.weight <= max_weight)
				|| ((max_sum - se.weight) <= (min_sum + se.weight)))
			continue;

		max_weight = (p->wrr).weight;
		move_p = p;
	}

	/* actually move the task */
	if (move_p && max_weight > 0) {
		BUG_ON(task_running(max_rq, p));
		deactivate_task(highest_rq, p, 0);
		set_task_cpu(p, min_cpu);
		activate_task(min_rq, p, 0);
	}

	double_rq_unlock(max_rq, min_rq);
}

void load_balance_wrr(void){
	
	int cpu, min_cpu, max_cpu; 
	int	min_weight, max_weight;
	int curr_weight;

	rcu_read_lock();

	//initializing
	min_weight = cpu_rq(task_cpu(current)) -> sum_weight;
	min_cpu = task_cpu(current);
	max_weight = cpu_rq(task_cpu(current)) -> sum_weight;
	max_cpu = task_cpu(current);
	//find smallest rq of online cpu
	for_each_online_cpu(cpu) {

		curr_weight = cpu_rq(cpu)->sum_weight
		if(curr_weight > max_weight){
			max_weight = curr_weight;
			max_cpu = cpu;
		}
		if(curr_weight < min_weight){
			min_weight = curr_weight;
			min_cpu = cpu;
		}
	}
	rcu_read_unlock();

	move_task(max_cpu, min_cpu);/
	
}

const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task	= enqueue_task_wrr,
	.dequeue_task	= dequeue_task_wrr,
	.yield_task		= yield_task_wrr,

	.check_preempt_curr	= check_preempt_curr_wrr,

	.pick_next_task	= pick_next_task_wrr,
	.put_prev_task	= put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq	= select_task_rq_wrr,
	
	.set_cpus_allowed	= set_cpus_allowed_wrr,
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
