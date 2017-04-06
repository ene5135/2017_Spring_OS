#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(){

	printf("%ld ", syscall(380, 0));
	printf("%ld ", syscall(381, 0, 0));
	printf("%ld ", syscall(382, 0, 0));
	printf("%ld ", syscall(383, 0, 0));
	printf("%ld\n", syscall(385, 0, 0));


	return 0;
}
