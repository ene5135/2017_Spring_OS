#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FACTOR 80000000
#define N 10

int main(int argc) {
	int i,j;
	int swap = 0;

	for (i=0; i<N; i++) {
		for (j=0; j<FACTOR; j++) {
			if (swap) swap--;
			else swap++;
		}
	}

	return 0;
}

