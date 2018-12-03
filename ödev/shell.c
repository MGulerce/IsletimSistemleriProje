#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <fcntl.h>
#define BUFSIZE 1000
#define INPBUF 100
#define ARGMAX 10
#define GREEN "\x1b[92m"
#define BLUE "\x1b[94m"
#define DEF "\x1B[0m"
#define CYAN "\x1b[96m"

struct _instr
{
    char * argdeger[INPBUF];
    int argsayac;
};
typedef struct _instr instruction;

char *giris,*giris1;
int cikisbayrak = 0;
int filepid,fd[2];
char cwd[BUFSIZE];
char* ardgeder[ARGMAX]; // argc, argv
int argsayac = 0,arkaplan = 0;
int externalIn=0,externalOut=0;
char girisDosya[INPBUF],cikisDosya[INPBUF];
void hakkinda();
void getInput();
int fonksiyon_exit();
void fonksiyon_pwd(char*, int);
void fonksiyon_cd(char*);
void fonksiyon_mkdir(char*);
void fonksiyon_rmdir(char*);
void fonksiyon_clear();
void dosyaAdi(struct dirent*, char*);
void fonksiyon_ls();
void fonksiyon_lsl();
void fonksiyon_cp(char*, char*);
void executable();
void pipe_dup(int, instruction*);
void run_process(int, int, instruction*);

void stopSignal()
{

    if(filepid!=0)
    {
        int temp = filepid;
        kill(filepid, SIGINT);
        filepid = 0;

    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT,stopSignal);
    int i;
    int pipe1 = pipe(fd);
    fonksiyon_clear();
    fonksiyon_pwd(cwd,0);

    while(cikisbayrak==0)
    {
        externalIn = 0; externalOut = 0;arkaplan = 0;
        printf(">",DEF,cwd ); // prompt 

        getInput();

        if(strcmp(ardgeder[0],"quit")==0 || strcmp(ardgeder[0],"z")==0)
        {
            fonksiyon_exit();
        }
        else if(strcmp(ardgeder[0],"hakkinda")==0 && !arkaplan)
        {
            hakkinda();
        }
        else if(strcmp(ardgeder[0],"pwd")==0 && !arkaplan)
        {
            fonksiyon_pwd(cwd,1);
        }
        else if(strcmp(ardgeder[0],"cd")==0 && !arkaplan)
        {
            char* path = ardgeder[1];
            fonksiyon_cd(path);
        }
        else if(strcmp(ardgeder[0],"mkdir")==0 && !arkaplan)
        {
            char* foldername = ardgeder[1];
            fonksiyon_mkdir(foldername);
        }
        else if(strcmp(ardgeder[0],"rmdir")==0 && !arkaplan)
        {
            char* foldername = ardgeder[1];
            fonksiyon_rmdir(foldername);
        }
        else if(strcmp(ardgeder[0],"clear")==0 && !arkaplan)
        {
            fonksiyon_clear();
        }
        else if(strcmp(ardgeder[0],"ls")==0 && !arkaplan)
        {
            char* optional = ardgeder[1];
            if(strcmp(optional,"-l")==0 && !arkaplan)
            {
                fonksiyon_lsl();
            }
            else fonksiyon_ls();
        }
        else if(strcmp(ardgeder[0],"cp")==0 && !arkaplan)
        {
            char* file1 = ardgeder[1];
            char* file2 = ardgeder[2];
            if(argsayac > 2 && strlen(file1) > 0 && strlen(file2) > 0)
            {
                fonksiyon_cp(file1,file2);
            }
            else
            {
                printf(" Kopyalama Hatasi: Yetersiz parametre\n");
            }
        }
        else
        {
            executable();
        }

    }

}


/*boþluk ve sekme içeren giriþleri al ve argval * içinde sakla*/
void getInput()
{
    fflush(stdout); //bufferlari temizleme
    giris = NULL;
    ssize_t buf = 0;
    getline(&giris,&buf,stdin);
    giris1 = (char *)malloc(strlen(giris) * sizeof(char));
    strncpy(giris1,giris,strlen(giris));
    argsayac = 0;arkaplan = 0;
    while((ardgeder[argsayac] = strsep(&giris, " \t\n")) != NULL && argsayac < ARGMAX-1)
    {
        if(sizeof(ardgeder[argsayac])==0)
        {
            free(ardgeder[argsayac]);
        }
        else argsayac++;
        if(strcmp(ardgeder[argsayac-1],"&")==0)
        {
            arkaplan = 1; //arka planda calisiyor.
            return;
        }
    }
    free(giris);
}



void runprocess(char * cli, char* args[], int count)
{
    int ret = execvp(cli, args);
    char* pathm;
    pathm = getenv("PATH");
    char path[1000];
    strcpy(path, pathm);
    strcat(path,":");
    strcat(path,cwd);
    char * cmd = strtok(path, ":\r\n");
    while(cmd!=NULL)
    {
       char loc_sort[1000];
        strcpy(loc_sort, cmd);
        strcat(loc_sort, "/");
        strcat(loc_sort, cli);
        printf("execvp : %s\n",loc_sort );
        ret = execvp(loc_sort, args);
        if(ret==-1)
        {
            perror(" Yürütülebilir dosya calistirilirken hata olustu. ");
            exit(0);
        }
        cmd = strtok(NULL, ":\r\n");
    }
}

//Boru
void pipe_dup(int n, instruction* komut)
{
    int in = 0,fd[2], i;
    int pid, durum,pipest;

    if(externalIn)
    {
        in = open(girisDosya, O_RDONLY); // dosya açma
        if(in < 0)
        {
            perror(" Yürütülebilir dosyada hata: giris dosyasi ");
        }
    }
    
    for (i = 1; i < n; i++)
    {
        pipe (fd);
        int id = fork();
        if (id==0)
        {
            if (in!=0)
            {
                dup2(in, 0);
                close(in);
            }
            if (fd[1]!=1)
            {
                dup2(fd[1], 1);
                close(fd[1]);
            }
            runprocess(komut[i-1].argdeger[0], komut[i-1].argdeger,komut[i-1].argsayac);
            exit(0);
        }
        else wait(&pipest);
        close(fd[1]);
        in = fd[0]; 
    }
    i--; 
    if(in != 0)
    {
        dup2(in, 0);
    }
    if(externalOut)
    {
        int ofd = open(cikisDosya, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(ofd, 1);
    }
    runprocess(komut[i].argdeger[0], komut[i].argdeger, komut[i].argsayac);
}



void executable()
{
    instruction command[INPBUF];
    int i=0,j=1,status;
	char* curr = strsep(&giris1, " \t\n");
                               
    command[0].argdeger[0] = curr;

    while(curr!=NULL)
    {
        curr = strsep(&giris1, " \t\n");
        if(curr==NULL)
        {
            command[i].argdeger[j++] = curr;
        }
        else if(strcmp(curr,"|")==0)
        {
            command[i].argdeger[j++] = NULL;
            command[i].argsayac = j;
            j = 0;i++;
        }
        else if(strcmp(curr,"<")==0)
        {
            externalIn = 1;
            curr = strsep(&giris1, " \t\n");
            strcpy(girisDosya, curr);
        }
        else if(strcmp(curr,">")==0)
        {
            externalOut = 1;
            curr = strsep(&giris1, " \t\n");
            strcpy(cikisDosya, curr);
        }
        else if(strcmp(curr, "&")==0)
        {
            arkaplan = 1;
        }
        else
        {
            command[i].argdeger[j++] = curr;
        }
    }
    command[i].argdeger[j++] = NULL; 
    command[i].argsayac = j;
    i++;

    // Ebeveyn proses çalismak için bekler sonra terminalden okur.
    filepid = fork();
    if(filepid == 0)
    {
        pipe_dup(i, command);
    }
    else
    {
        if(arkaplan==0)
        {
            waitpid(filepid,&status, 0);
        }
        else
        {
            printf("Proses arka planda calisiyor. PID:%d\n",filepid);
        }
    }
    filepid = 0;
    free(giris1);
}


/* kopyalama */
void fonksiyon_cp(char* dosya1, char* dosya2)
{
    FILE *f1,*f2;
    struct stat t1,t2;
    f1 = fopen(dosya1,"r");
    if(f1 == NULL)
    {
        perror("Dosya 1 kopyalama hatasi. ");
        return;
    }
    f2 = fopen(dosya2,"r");
    if(f2)
    {
        stat(dosya1, &t1);
        stat(dosya2, &t2);
        if(difftime(t1.st_mtime,t2.st_mtime) < 0)
        {
            printf("Kopyalama Hatasi : %s is more recently updated than %s\n",dosya2,dosya1);
            fclose(f1);
            fclose(f2);
            return;
        }
    }
    f2 = fopen(dosya2,"ab+"); // Mevcut degilse dosya yarat.
    fclose(f2);

    f2 = fopen(dosya2,"w+");
    if(f2 == NULL)
    {
        perror("Dosya 2 kopyalama hatasi ");
        fclose(f1);
        return;
    }
    if(open(dosya2,O_WRONLY)<0 || open(dosya1,O_RDONLY)<0)
    {
        perror("Hata");
        return;
    }
    char cp;
    while((cp=getc(f1))!=EOF)
    {
        putc(cp,f2);
    }

    fclose(f1);
    fclose(f2);
}



void dosyaAdi(struct dirent* name,char* followup)
{
    if(name->d_type == DT_REG)          // normal dosya
    {
        printf("%s%s%s",BLUE, name->d_name, followup);
    }
    else if(name->d_type == DT_DIR)    // klasör
    {
        printf("%s%s/%s",GREEN, name->d_name, followup);
    }
    else                              // bilinmeyen dosya tüleri
    {
        printf("%s%s%s",CYAN, name->d_name, followup);
    }
}


void fonksiyon_lsl()
{
    int i=0,total=0;
    char timer[14];
    struct dirent **listr;
    struct stat details;
    int listn = scandir(".",&listr,0,alphasort);
    if(listn > 0)
    {
        printf("%s+--- Klasördeki toplam obje sayisi %d \n",CYAN,listn-2);
        for ( i = 0; i < listn; i++)
        {
            if(strcmp(listr[i]->d_name,".")==0 || strcmp(listr[i]->d_name,"..")==0)
            {
                continue;
            }
            else if(stat(listr[i]->d_name,&details)==0)
            {
              
                total += details.st_blocks; 

                printf("%s%1s",DEF,(S_ISDIR(details.st_mode)) ? "d" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IRUSR) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWUSR) ? "w" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IXUSR) ? "x" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IRGRP) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWGRP) ? "w" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IXGRP) ? "x" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IROTH) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWOTH) ? "w" : "-");
                printf("%s%1s ",DEF,(details.st_mode & S_IXOTH) ? "x" : "-");
               
                printf("%2ld ",(unsigned long)(details.st_nlink));
                printf("%s ",(getpwuid(details.st_uid))->pw_name);
                printf("%s ",(getgrgid(details.st_gid))->gr_name);
      
                printf("%5lld ",(unsigned long long)details.st_size);
                strftime (timer,14,"%h %d %H:%M",localtime(&details.st_mtime));
                printf("%s ",timer);
                dosyaAdi(listr[i],"\n");
            }
        }
        printf("%s+--- Toplam nesne içeriği %d\n",CYAN,total/2);
    }
    else
    {
            printf(" Bos dosya\n" );
    }
}


void fonksiyon_ls()
{
    int i=0;
    struct dirent **listr;
    int listn = scandir(".", &listr, 0, alphasort);
    if (listn >= 0)
    {
        printf("%s Dosya içerisindeki toplam obje sayisi %d \n",CYAN,listn-2);
        for(i = 0; i < listn; i++ )
        {
            if(strcmp(listr[i]->d_name,".")==0 || strcmp(listr[i]->d_name,"..")==0)
            {
                continue;
            }
            else dosyaAdi(listr[i],"    ");
            if(i%8==0) printf("\n");
        }
        printf("\n");
    }
    else
    {
        perror ("ls hatasi ");
    }

}


/* ekrani temizler*/
void fonksiyon_clear()
{
    const char* blank = "\e[1;1H\e[2J";
    write(STDOUT_FILENO,blank,12);
}

/* dosya siler */
void fonksiyon_rmdir(char* name)
{
    int statrm = rmdir(name);
    if(statrm==-1)
    {
        perror("rmdir hata ");
    }
}


/* dosya oluþturma */
void fonksiyon_mkdir(char* name)
{
    int stat = mkdir(name, 0777);
    if(stat==-1)
    {
        perror("mkdir hata ");
    }
}


/*dizinler arasý geçiþ*/
void fonksiyon_cd(char* path)
{
    int ret = chdir(path);
    if(ret==0)
    {
        fonksiyon_pwd(cwd,0);
    }
    else perror("+--- Hata: cd ");
}


/*çýkýþ*/
int fonksiyon_exit()
{
    cikisbayrak = 1;
    return 0; 
}

/* geçerli dizini ekrana yazar*/
void fonksiyon_pwd(char* cwdstr,int command)
{
    char temp[BUFSIZE];
    char* path=getcwd(temp, sizeof(temp));
    if(path != NULL)
    {
        strcpy(cwdstr,temp);
        if(command==1)  
        {
            printf("%s\n",cwdstr);
        }
    }
    else perror("Hata: getcwd()  ");

}

/* kabuk hakkýnda*/
void hakkinda()
{
    
    char* yazi = "Bu kabuk .... grup tarafýndan kodlandý. ";

    printf("%s",yazi);
}
