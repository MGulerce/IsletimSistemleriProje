#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define SEMKEY 1234
int sonsem;


void signal12(void)
{
}



void sem_signal(int semid, int val) {
	struct sembuf semafor;
	semafor.sem_num = 0;
	semafor.sem_op = val;
	semafor.sem_flg = 0;
	semop(semid, &semafor, 1);
}



void sem_wait(int semid, int val)
{
	struct sembuf semafor;
	semafor.sem_num = 0;
	semafor.sem_op = (-1 * val);
	semafor.sem_flg = 0;
	semop(semid, &semafor, 1);
}


int main(void)
{
	int f;
	sem = semget(SEMKEY, 1, 0700 | IPC_CREAT);
	semctl(sem, 0, SETVAL, 0);
	f = fork();
	if (f == -1)
	{
		printf("fork error\n");
		exit(1);
	}
	if (f > 0) /*anne */
	{
		printf(“Anne calışmaya başladı…\n”);
		sem_wait(sem, 10);
		printf("cocuk semaforu artırdı\n");
		semctl(sem, 0, IPC_RMID, 0);
	}
	else /*cocuk */
	{
		printf(“ Cocuk çalışmaya başladı…\n”);
		sem = semget(SEMKEY, 1, 0);
		sem_signal(sem, 1);
		printf(” Cocuk : semafor değeri = %d\n”,
			semctl(sonsem, 0, GETVAL, 0));
	}
	return(0);
}

