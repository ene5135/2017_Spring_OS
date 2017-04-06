
#include<linux/rotation.h>
//#include<asm/uaccess.h>
#include<linux/printk.h>
#include<linux/spinlock.h>

/*
 * set_rotation() simply copy the degree from user space to kernel space.
 * global_rotation is declared in rotation.h
 * after copying the degree, it calls common function to let other processes * know the new rotation and be rescheduled.
 *
 */


/* initialize global variables */

int global_rotation = 0;

struct proc_lock_info * waiting_list;
struct proc_lock_info * acquiring_list;

DEFINE_SPINLOCK(global_lock);


asmlinkage long sys_set_rotation(int degree)
{

	spin_lock(&global_lock);

	global_rotation = degree;
	
	printk("Current degree is %d\n",global_rotation);

	rescheduler();

	spin_unlock(&global_lock);
	
	return 0;
}

/*
 * common function that manages all shared variables.
 * this function should 
 * 1. update all shared variables we use.----------- X !!it will be done in caller function
 * 2. reschedule processes that are waiting or running. (Maybe using signal)
 *
 */

void rescheduler()
{

	1. updating part ------------ X !!it will be done in caller function

	2. rescheduling part
	struct proc_lock_info * cursor;
	int lower = 0, upper = 0;
	macro(cursor) // traverse waiting list
	{
		cursor->degree - cursor->range = lower;
		cursor->degree + cursor->range = upper;
		if(cursor->
	}	
	

}


