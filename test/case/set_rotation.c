#include<stdio.h>
#include<string.h>

int main(int argc, char* argv[]) 
{

	syscall(380,atoi(argv[1]));

	return 0;
}
