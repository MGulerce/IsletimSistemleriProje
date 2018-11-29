#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMKEY 2367
int main (void)
{
	int *pb;
	int pbid;
	int i;
	pbid=shmget(SHMKEY,sizeof(int),IPC_CREAT|0700);
	pb=(int *)shmat(pbid,0,0);
	*pb=0;
	
	for(i=0; i<10; i++)
	{
		(*pb)++;
		printf("yeni değer: %d\n",(*pb));

	}

	shmdt( pb);
	return 0;

}
