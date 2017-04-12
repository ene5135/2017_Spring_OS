#include<linux/sched.h>

#define _WRITE 1
#define _READ 0

struct proc_lock_info
{
	struct task_struct * task;
	int range;
	int degree;
	int type;
	struct list_head sibling;
};

void rescheduler(void);

int check_acquiring_list(struct proc_lock_info * new_proc);
int check_waiting_write(void);
int is_writer(struct proc_lock_info * pli);
int is_overwrapped(int d1, int r1, int d2, int r2);
int is_in_range(int degree, int range, int rotation);
void exit_rotlock(void);
