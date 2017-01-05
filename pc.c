#define   __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

_syscall2(sem_t*,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t*,sem);
_syscall1(int,sem_post,sem_t*,sem);
_syscall1(int,sem_unlink,const char *,name);

#define NUMBER 1000
#define CHILD 5
#define BUFSIZE 10

sem_t   *empty, *full, *mutex;
int fno;

int main()
{
    int  i,j,k;
    int  data;
    pid_t p;
    int  buf_out = 0;
    int  buf_in = 0;
    if((mutex = sem_open("carmutex",1)) == SEM_FAILED)
    {
        perror("sem_open() error!\n");
        return -1;
    }
    if((empty = sem_open("carempty",10)) == SEM_FAILED)
    {
        perror("sem_open() error!\n");
        return -1;
    }
    if((full = sem_open("carfull",0)) == SEM_FAILED)
    {
        perror("sem_open() error!\n");
        return -1;
    }
    fno = open("buffer.dat",O_CREAT|O_RDWR|O_TRUNC,0666);
    lseek(fno,10*sizeof(int),SEEK_SET);
    write(fno,(char *)&buf_out,sizeof(int));
    if((p=fork())==0)
    {
        for( i = 0 ; i < NUMBER; i++)
        {
            sem_wait(empty);
            sem_wait(mutex);
            lseek(fno, buf_in*sizeof(int), SEEK_SET);
            write(fno,(char *)&i,sizeof(int));
            buf_in = ( buf_in + 1)% BUFSIZE;

            sem_post(mutex);
            sem_post(full);
        }
        return 0;
    }
    else if(p < 0)
    {
        perror("Fail to fork!\n");
        return -1;
    }

    for( j = 0; j < CHILD ; j++ )
    {
        if((p=fork())==0)
        {
            for( k = 0; k < NUMBER/CHILD; k++ )
            {
                sem_wait(full);
                sem_wait(mutex);
                lseek(fno,10*sizeof(int),SEEK_SET);
                read(fno,(char *)&buf_out,sizeof(int));
                lseek(fno,buf_out*sizeof(int),SEEK_SET);
                read(fno,(char *)&data,sizeof(int));
                buf_out = (buf_out + 1) % BUFSIZE;
                lseek(fno,10*sizeof(int),SEEK_SET);
                write(fno,(char *)&buf_out,sizeof(int));
                sem_post(mutex);
                sem_post(empty);
                printf("%d:  %d\n",getpid(),data);
                fflush(stdout);
            }
            return 0;
        }
        else if(p<0)
        {
            perror("Fail to fork!\n");
            return -1;
        }
    }
    wait(NULL);
    sem_unlink("carfull");
    sem_unlink("carempty");
    sem_unlink("carmutex");
    close(fno);
    return 0;
}
