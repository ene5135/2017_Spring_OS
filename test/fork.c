#include<unistd.h>

void main()
{
	int i=0;
	char t;
	pid_t pid = fork();
	if(pid)
		t = 'A';
	else
		t = 'B';
	
	while(1)
	{
		i++;
		if(i == 1)
		{
			printf("%c\n",t);
			i=0;
		}
	}

}
