#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>

void main(int argc, char **argv){

	pid_t pid = fork();
	long r;
	char * arg[2] = {NULL, NULL};
	if(pid)
	{
		r=syscall(380, pid, atoi(argv[1]));
		waitpid(-1,NULL,0);
		printf("%ld\n", r);
	}
	else{
		execv("./trial", arg);
	}
}
