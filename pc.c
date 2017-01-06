#define   __LIBRARY__
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

_syscall2(sem_t  *, sem_open, const char*, name, unsigned int , value)
_syscall1(int, sem_wait, sem_t  *, sem)
_syscall1(int, sem_post, sem_t  *, sem)
_syscall1(int, sem_unlink, const char  *, name)

int main (){
    const int NUM = 500;
    const int p_NUM = 5;
    FILE * fp;
    int buf[10],i,j=0,k=1;
    int n,out;
    sem_t *full, *empty, *mutex;
    full = sem_open("full",0);
    empty = sem_open("empty",10);
    mutex = sem_open("mutex",1);

    if (!fork())
    {
        for (i = 0;i < NUM;i ++)
        {
      	    sem_wait(empty);
      	    sem_wait(mutex);
      	    fp = fopen("buffer.txt","ab+");
      	    fwrite(&i,sizeof(int),1,fp);
      	    fflush(fp);
      	    fclose(fp);
      	    sem_post(mutex);
      	    sem_post(full);
      	}
	       exit(0);
    }
    for (i = 0;i < p_NUM;i++)
    {
	     if (!fork())
        {
	         for (n = 0;n < NUM/p_NUM;n ++)
            {
                sem_wait(full);
                sem_wait(mutex);
                fflush(stdout);
                fp = fopen("buffer.txt","rb+");
                j=0;

                while(j<10){
                    out = -1;
                    fread(&out,sizeof(int),1,fp);
                    buf[j] = out;
                    j++;
                }
		        fclose(fp);

		        printf (" %d :%d \n",getpid(),buf[0]);
		        fflush(stdout);
		        fp = fopen("buffer.txt","wb");

                k=1;
                while(k<10){
                    if (buf[k] > 0)
                        fwrite((buf+k),sizeof(int),1,fp);
                    k++;
                }
                fflush(fp);
                fclose(fp);
                sem_post(mutex);
                sem_post(empty);
	        }
	        exit(0);
        }
    }
    wait(NULL);
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("mutex");
    return 0;
}
