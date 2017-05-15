#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FACTOR 80000000
#define FORK 1000

int main(int argc, char **argv) {
	int i;
	int swap = 0;
	
	for (i=0; i<FACTOR; i++) {
		if (swap) swap--;
		else swap++;
	}

	printf("short terminated\n");

	return 0;
}

