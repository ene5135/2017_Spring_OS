#include<stdio.h>
#include<unistd.h>

void main()
{
	int res = 0;
	syscall(382, 15, 15); //write_lock
	syscall(382, 46, 15);
	syscall(382, 77, 15);
		
	res = syscall(385, 46, 46); //write_unlock;
	
	printf("res = %d\n", res);
	return;
}
