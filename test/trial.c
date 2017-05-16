#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_factor(int n);

int main(int argc, char* argv[]) 
{

	int n;
	char str[20];
	FILE *fp;
	clock_t start,end;
	
	fp = fopen("integer", "r");

//	while (1) {
	
		start = clock();		
		fread(str, sizeof(str), 1, fp);
		n = atoi(str);
		printf("trial: %d = ", n);
		print_factor(n);
		end = clock();
//		printf("Calculation done : %f sec\n",(double)(end-start)/CLOCKS_PER_SEC);
//	}
	
	fclose(fp);
	return 0;
}

void print_factor(int n)
{
	int i;
	for (i=2; i<n; i++) {
		if ((n % i) == 0) {
			printf("%d * ", i);
			print_factor(n / i);
			return;
		}
	}
	printf("%d\n", n);
	return;
}
