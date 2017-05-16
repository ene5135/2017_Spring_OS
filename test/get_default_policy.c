#include<sched.h>
#include<stdio.h>
#include<stdlib.h>
#include<uapi/linux/sched.h>

int main()
{
        struct sched_param param, new_param;

	    printf("Default policy = %d \n", sched_getscheduler(0));

        return 0;
}

