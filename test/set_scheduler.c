#include<sched.h>
#include<stdio.h>
#include<stdlib.h>
#include<uapi/linux/sched.h>

int main(int argc, char** argv)
{
        struct sched_param param, new_param;
		int policy = atoi(argv[1]);

	    printf("Default policy = %d \n", sched_getscheduler(0));
/*
        param.sched_priority = (sched_get_priority_min(policy) +
                        sched_get_priority_max(policy)) /2;

        if(sched_setscheduler(0, policy, &param) != 0){
                perror("sched_setscheduler failed \n");
                return 0;
        }
		printf("Changed policy = %d \n", sched_getscheduler(0));
		
		printf("Task with policy %d is working well!\n",sched_getscheduler(0));
		while(1){}	
*/
        return 0;
}

