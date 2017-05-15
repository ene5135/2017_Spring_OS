#include<stdio.h>

int main(int argv, char *argc[]){


	long a = syscall(380, atoi(argc[1]), atoi(argc[2]));
	printf("%ld\n", a);
	return 0;
}
