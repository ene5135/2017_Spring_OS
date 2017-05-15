#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FACTOR 80000000
#define N 10
#define FORK FACTOR / 40 * N

int main(int argc) {
	int i,j;
	int swap = 0;
	pid_t pid;
	char **input = {{ NULL }};
	int n=0;

	for (i=0; i<N; i++) {
		for (j=0; j<FACTOR; j++) {
			if (swap) swap--;
			else swap++;
			if ((j%FORK)==0) {
				printf("%d", n++);
				pid = fork();
				if (!pid) execv("./short", input);
			}
		}
	}
	printf("parent\n");
	return 0;
}

