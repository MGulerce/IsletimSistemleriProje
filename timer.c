#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

void timer_callback(int signum)
{
	time_t now=time(NULL);
	printf("saat %li de sinyal %d yakalandı\n", now,signum);
}

int main()
{
	signal(SIGALRM, timer_callback);
	alarm(5);
	sleep(3);
	return 0;
}
