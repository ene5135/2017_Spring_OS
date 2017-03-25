#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/prinfo.h>
#include <linux/types.h>
#include <asm-generic/uaccess.h>
#include <linux/list.h>

void pre_order(struct prinfo *buf_kernel, int *buf_size, struct task_struct *curr, int *index);

asmlinkage long sys_ptree(struct prinfo *buf, int *nr){
	//error check. check check I'm the korean top class hip-hop nobless
	if(buf == NULL || nr == NULL)
		return -EINVAL;
	if(!access_ok(VERIFY_WRITE, nr, sizeof(int)))
		return -EFAULT;
	if(*nr < 1)
		return -EINVAL;
	if(!access_ok(VERIFY_WRITE, buf, (*nr)*sizeof(struct prinfo)))
		return -EFAULT;

	struct prinfo *buf_kernel = kmalloc( (*nr)*sizeof(struct prinfo), GFP_KERNEL);
	int count_process=0;
	
	read_lock(&tasklist_lock);
	pre_order(buf_kernel, nr, &init_task,&count_process); 
	read_unlock(&tasklist_lock);	
	
	//error check(copy_to_user)
	if(copy_to_user(buf, buf_kernel, (*nr)*sizeof(struct prinfo)))
	{
		kfree(buf_kernel);
		return 0;
	}
	if((*nr) > count_process)
	{
		if(copy_to_user(nr, &count_process, sizeof(int)))
		{
			kfree(buf_kernel);
			return 0;
		}
	}
	
	kfree(buf_kernel);
	return count_process;
}

void pre_order(struct prinfo *buf_kernel, int *buf_size, struct task_struct *curr, int *index){
	
	struct task_struct* curr_first_child = list_entry((curr->children).next, struct task_struct, sibling);
	struct task_struct* curr_next_sibling = list_entry((curr->sibling).next, struct task_struct, sibling);
	struct list_head *cursor;

	if( (*index) < (*buf_size)){
		//copy to task_struct -> prinfo
		buf_kernel[(*index)].state = curr->state;
		buf_kernel[(*index)].pid = curr->pid;
		buf_kernel[(*index)].parent_pid = (curr-> parent)->pid; 
		buf_kernel[(*index)].next_sibling_pid = curr_next_sibling->pid;
		buf_kernel[(*index)].uid = current_uid();
		strncpy(buf_kernel[(*index)].comm, curr->comm, TASK_COMM_LEN);
	}
	// if curr == last sibling
	if( ((curr->sibling).next) == &((curr->parent)->children)){
		if( (*index) < (*buf_size))
			buf_kernel[(*index)].next_sibling_pid = (curr->parent)->pid;
	}
	if(list_empty(&(curr->children))){
		if( (*index) < (*buf_size))
			buf_kernel[(*index)].first_child_pid = 0;
		(*index)++;
		return;
	}
	else{
		if( (*index) < (*buf_size))
			buf_kernel[(*index)].first_child_pid = curr_first_child->pid;
		(*index)++;
	}

	//call sibling
	list_for_each(cursor, &(curr->children)){
		pre_order(buf_kernel, buf_size, (list_entry(cursor, struct task_struct, sibling)), index);
	}
}
