#include<stdio.h>

int main() {
	int a = 0;
	while(1)
	{
		a++;
		if (a==10000000)
		{
			a=0;
			printf("A\n");
		}
	}
	return 0;
}
