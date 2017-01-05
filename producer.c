#define   __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define NUM  550
#define KEY  10
#define PAGE  4096

_syscall2(int,sem_open, const char*, name, unsigned int , value)
_syscall1(int, sem_wait, sem_t  *, sem)
_syscall1(int, sem_post, sem_t  *, sem)
_syscall1(int, sem_unlink, const char  *, name)
_syscall2(int, shmget, key_t, key, size_t, size)
_syscall2(void *, shmat, int, phyAddr, const void*, shmaddr)


int main()
{
    int i, flag, phyAddr,* shmaddr;
    sem_t *empty, *full, *mutex;
    empty = (sem_t *)sem_open("empty",10);
    full  = (sem_t *)sem_open("full", 0);
    mutex = (sem_t *)sem_open("mutex", 1);
    phyAddr = shmget((key_t) KEY,PAGE);
    shmaddr = (int*)shmat(phyAddr,NULL);
    for( i = 0 ; i < NUM; i++) {
        sem_wait(empty);
        sem_wait(mutex);
        shmaddr[i%10] = i;
        sem_post(mutex);
        sem_post(full);
    }
    fflush(stdout);
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");
    return 0;
}
