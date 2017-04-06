#include<linux/types.h>

struct proc_lock_info
{
	pid_t pid;
	int range;
	int degree;
	int type;
	struct list_head sibling;
};

int set_rotation(int degree);	/* 0 <= degree < 360 */
void rescheduler();
