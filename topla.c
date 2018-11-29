#include <stdio.h>
#include <stdlib.h>

int main(int argsay, char * argvek[])
{
	int toplam;
	if (argsay<3)
	{
		printf("eksik parametre\n");
		exit(0);
	}
	if (argsay>3)
	{
		printf("fazla parametre\n");
		exit(0);
	}
 
	toplam= atoi(argvek[1])+atoi(argvek[2]);
	printf("toplam: %d\n",toplam);
return 0;


}
