#include <linux/unistd.h>
#include <linux/list.h>

#define READ 0
#define WRITE 1

struct proc_lock_info {
	pid_t pid;
	list_head * next;
	int degree;
	int rotation;
	int type;
};

