#include<stdio.h>

int main(int argv, char** argc){

	long a = syscall(381, atoi(argc[1]));
	printf("%ld\n", a);
	return 0;

}
