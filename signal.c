#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

void my_func(int value)
{
	printf("yakalanan sinyal :%d\n",value);
	exit(1);
}

int main (int argc, char ** argv)
{
	signal(SIGINT,my_func);
	while(1);
	return 0;
}


