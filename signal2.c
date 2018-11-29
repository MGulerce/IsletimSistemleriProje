#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void signal12(void)
{
	printf("12 nolu sinyali aldım.\n");
	exit(0);
}

int main(void)
{
	int f;
	signal(12,(void *)signal12);
	f=fork();
	if(f==-1)
	{
		printf("fork hatası\n");
	}
	if(f==0)
	{
		printf("çocuk başladı\n");
		pause();

	}
	else
	{
		printf("Anne: başlıyorum\n");
		sleep(3);
		printf("anne: çocuğa sinyal yolluyorum\n");
		kill(f,12);
		exit(0);
		
	}
	return 0;
}
