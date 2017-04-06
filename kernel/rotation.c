#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/rotation.h>
#include <asm-generic/uaccess.h>


static int degree = 0;

asmlinkage long sys_set_rotation(int degree) 
{
	printk("sys_set_rotation(380) called\n");
	return count+=1;
	// return total # of awaken processes
	// return -1 on error
}

asmlinkage long sys_rotlock_read(int degree, int range) 
{
	printk("sys_rotlock_read(381) called\n");
	return count+=2;
}

asmlinkage long sys_rotlock_write(int degree, int range) 
{
	printk("sys_rotlock_write(382) called\n");
	return count+=3;
}

asmlinkage long sys_rotunlock_read(int degree, int range) 
{
	printk("sys_rotunlock_read(383) called\n");
	return count+=4;
}

asmlinkage long sys_rotunlock_write(int degree, int range) 
{
	printk("sys_rotunlock_write(385) called\n");
	return count+=5;
}
