#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm-generic/uaccess.h>
#include <linux/list.h>

asmlinkage long sys_sched_setweight(pid_t pid, int weight)
{
	return 1;
}

asmlinkage long sys_sched_getweight(pid_t pid)
{
	return 1;
}
