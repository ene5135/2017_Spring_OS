/////// shinhwi ///////

/////// check ///////
#define QUANTUM 

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	// no idea about flags
	struct wrr_rq *target_wrr_rq = &(rq -> wrr);
	struct sched_wrr_entity *wrr_se  = &(p-> wrr);

	// add into wrr_rq
	list_add_tail( &(wrr_se->run_list), &(target_wrr_rq->queue_head));
	// increase sum_weight
	target_wrr_rq->sum_weight += wrr_se->weight;

	/////// check ///////
	// no idea about initialize
	// no idea about on_running
	// ambiguous
	if(task_current(rq, p)){
		wrr_se -> on_running = 1;
	} 
	else{
		wrr_se -> on_running = 0;
	}
	// increase nr_running of rq
	inc_nr_running(rq);
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){

	struct wrr_rq *target_wrr_rq = &(rq-> wrr);
	struct sched_wrr_entity *wrr_se = &(p->wrr);

	// delete from wrr_rq
	list_del(&(wrr_se->run_list));

	// decrease sum_weight
	target_wrr_rq->sum_weight -= wrr_se->weight;
	
	// dereasse nr_running of rq
	dec_nr_running(rq);
}

static void yield_task_wrr(struct rq *rq){
	
	struct wrr_rq *target_wrr_rq = &(rq->wrr);
	struct list_head *target =  &(((rq->curr)->wrr).run_list);
	// move first to tail
	list_move_tail(target, &(target_wrr_rq->queue_head));
}

static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags){
}

static struct task_struct *get_task_from_wrr_se(struct sched_wrr_entity *wrr_se){
	return container_of(wrr_se, struct task_struct, wrr);
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
	
	next_wrr_se->timeslice = next_wrr_se->weight*QUANTUM;
	next_wrr_se->on_running = 1;
	return get_task_from_wrr_se(next_wrr_se);
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
#endif

	.set_curr_task	= set_curr_task_wrr,
	.task_tick		= task_tick_wrr,

	.get_rr_interval= get_rr_interval_wrr,

	.prio_changed	= prio_changed_wrr,
	.switched_to	= switched_to_wrr
};
