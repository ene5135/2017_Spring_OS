#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main(int argc, char* argv[]) 
{
	if (argc != 6) {
		printf("invalid input\n");
		return 0;
	}

// proc_ID lock_type, degree, range, sleep_time

	int sleep_time = atoi(argv[5]);
	char lock_type = argv[2][0];
	int degree = atoi(argv[3]);
	int range = atoi(argv[4]);
	int proc_ID = atoi(argv[1]);

	int lock=0,unlock=0;
	if(lock_type == 'R')
	{
		lock = 381;
		unlock = 383;
	}
	else if(lock_type == 'W')
	{
		lock = 382;
		unlock = 385;
	}

	syscall(lock, degree, range); 
	printf("################# Proc[%d] acquire lock : %c, %d, %d\n",proc_ID,lock_type,degree,range);

	sleep(sleep_time);

	printf("################# Proc[%d] release lock : %c, %d, %d\n",proc_ID,lock_type,degree,range);
	syscall(unlock, degree, range);	
	
	return 0;
}
