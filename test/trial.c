#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_factor(int n);

int main(int argc, char* argv[]) 
{
	if (argc != 2) {
		printf("need an integer\n");
		return 0;
	}

	int id = atoi(argv[1]);
	int n;
	char str[20];
	FILE *fp;

	while (1) {
		syscall(381, 90, 90); // call sys_rotlock_read(90, 90)

		fp = fopen("integer", "r");
		fread(str, sizeof(str), 1, fp);
		n = atoi(str);
		printf("trial-%d: %d = ", id, n);
		print_factor(n);
		fclose(fp);

		syscall(383, 90, 90); // call sys_rotunlock_read(90, 90);
	}
	
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
