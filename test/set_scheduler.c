#include<sched.h>
#include<stdio.h>
#include<uapi/linux/sched.h>
int main()
{
        struct sched_param param, new_param;
        printf("start policy = %d \n", sched_getscheduler(0));

        param.sched_priority = (sched_get_priority_min(SCHED_NORMAL) +
                        sched_get_priority_max(SCHED_NORMAL)) /2;
        printf("max priority = %d, min priority = %d, my priority = %d \n", sched_get_priority_max(SCHED_NORMAL), sched_get_priority_min(SCHED_NORMAL),
                        param.sched_priority);

        if(sched_setscheduler(0, SCHED_NORMAL, &param) != 0){
                perror("sched_setscheduler failed \n");
                return 0;
        }

        /* 이 부분에 타임 크리티컬한 연산을 넣는다. */
		while(1){}

        sched_yield();  /*다른 실시간 쓰레드, 프로세스가 수행될 수 있도록 CPU를 양도한다. 
                          이것이 필요한 이유는 이 프로세스가 실시간 선점형 프로세스이기때문에 
                          다른 일반 프로세스, 쓰레드들을 항상 선점하기 때문이다.(기아현상 방지)*/

        param.sched_priority = sched_get_priority_max(SCHED_NORMAL);
        if(sched_setparam(0, &param) != 0){
                perror("sched_setparam failed \n");
                return 0;
        }

        sched_getparam(0, &new_param);

        printf("I am running at priority %d \n",
                        new_param.sched_priority);

        return 0;
}

