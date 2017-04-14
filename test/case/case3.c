#include<stdio.h>
#include<unistd.h>

void main()
{
	int res = 0;

	syscall(382, 150, 30); //write_lock
	printf("process1 acquires write_lock [120,180]\n");
	
	if(fork() == 0)
	{
		syscall(382,90,30);
		printf("process2 acquires write_lock [60,120]\n");
		sleep(3);
		syscall(383,80,20);
		printf("process3 releases read_lock [60,100]\n");
		return;
	}
	
	if(fork() == 0)
	{
		syscall(381,80,20);
		printf("process3 acquires read_lock [60,100]\n");
		sleep(3);
		syscall(383,80,20);
		printf("process3 releases read_lock [60,100]\n");
		return;
	}

	
	if(fork() == 0)
	{
		syscall(381,80,20);
		printf("process4 acquires read_lock [60,100]\n");
		return;
	}

	syscall(383,150,30);
	printf("process1 releases write_lock [120,180]\n");

	return;
}
