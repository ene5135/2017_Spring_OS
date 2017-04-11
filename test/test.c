#include <stdio.h>
#include <wait.h>
#include <current.h>

int main(){

	
	DECLARE_WAITQUEUE(wait,current);
	printf("Good night\n");
	wait_event(wait,0);
	printf("Good moring\n");


	return 0;
}
