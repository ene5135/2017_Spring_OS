#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FACTOR 80000000
#define FORK 1000

int main(int argc, char **argv) {
	int n = atoi(argv[1]);
	int i,j;
	int swap = 0;
	pid_t pid;
	char **input = {"1"};

	for (i=0; i<n; i++) {
		for (j=0; j<FACTOR; j++) {
			if (!(j%FORK)) {
				pid = fork();
				if (!pid) execve("./sec", input);
			if (swap) swap--;
			else swap++;
		}
	}

	return 0;
}

