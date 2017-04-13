#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) 
{
	if (argc < 4) {
		printf("Give me ID, Degree, Range\n");
		return 0;
	}

	int n = atoi(argv[1]);
	int degree = atoi(argv[2]);
	int range = atoi(argv[3]);
	char str[20];
	FILE *fp;

	while (1) {
		syscall(382, degree, range); // call sys_rotlock_write(90, 90)

		printf("selector: %d\n", n);
		fp = fopen("integer", "w+");
		sprintf(str, "%d", n++);
		fwrite(str, strlen(str) * sizeof(*str), 1, fp);
		fclose(fp);
		syscall(385, degree, range); // call sys_rotunlock_write(90, 90);
	}
	
	return 0;
}
