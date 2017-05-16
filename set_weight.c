#include<stdio.h>
#include<stdlib.h>
#include<asm-generic/current.h>

void main(int argv, char** argc)
{
	if(syscall(380,atoi(argc[1]),atoi(argc[2])) < 0)
		printf("error!\n");
}
