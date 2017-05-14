#include<linux/sched.h>
#include<uapi/linux/sched.h>
#include<linux/types.h>
#include<asm-generic/current.h>
#include<uapi/asm-generic/errno-base.h>
#include<linux/cred.h>
#include<linux/rcupdate.h>

#include "../kernel/sched/sched.h"


// I don't know what error code should be used,,, I just used -EINVAL in all cases
asmlinkage long sys_sched_setweight(pid_t pid, int weight)
{
	return sched_setweight(pid,weight);	
}


// should I get a lock in the sched_getweight???? // geungook
asmlinkage long sys_sched_getweight(pid_t pid)
{
	return sched_getweight(pid);
}
