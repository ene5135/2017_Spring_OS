#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

void main()
{
	int res = 0;
	syscall(382, 30, 30); //write_lock
	printf("proc1 ac wl\n");
	syscall(381, 150, 30); // read_lock
	printf("proc1 ac rl\n");

	if(fork() == 0)
	{
		printf("child1\n");
		syscall(382,30,30);
		printf("process2 acquires lock, this should be shown after process1 terminates\n");
		return;
	}
	if(fork() == 0)
	{
		printf("child2\n");
		syscall(382,135,15);
		printf("process3 acquires lock, this should be shown after process1 terminates\n");
		return;
	}

	sleep(10);
	printf("process1 terminates\n");
	exit(0);
	return;
}
