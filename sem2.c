#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>

#define SEMKEY 2345
void sem_signal(int semid, int val)
{
	struct sembuf semafor;
	semafor.sem_num=0;
	semafor.sem_op=val+1;
	semafor.sem_flg=0;
	semop(semid,&semafor,1);

}

void sem_wait(int semid,int val)
{
	struct sembuf semafor;
	semafor.sem_num=0;
	semafor.sem_op=(-1*val);
	semafor.sem_flg=0;
	semop(semid,&semafor,1);
	
}

int sem;
int sonsem;
int main (void)
{
	sem=semget(SEMKEY, 1,IPC_CREAT|0700);
	semctl(sem,0,SETVAL,0);
	
	int f;
	f=fork();
	if(f==-1)
	{
		printf("proses oluşmadı!");
		exit(1);
	}
	
	if(f>0)
	{
		printf("anne çalışmaya başladı!\n");
		sem_wait(sem,1);
		printf("çocuk semaforu arttırdı\n");
		semctl(sem,0,IPC_RMID,0);
		
	}
	else
	{
		printf("çocuk çalışmaya başladı!\n");
		sem=semget(SEMKEY,1,0);
		sem_signal(sem,1);
		printf("çocuk semafor değeri: %d\n", semctl(sem,0,GETVAL,0));

	}

	return (0);
}

















