#define   __LIBRARY__
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

_syscall2(sem_t  *, sem_open, const char*, name, unsigned int , value)
_syscall1(int, sem_wait, sem_t  *, sem) /* 等待信号量至其值大于0，将其值减1；对应P原语 */
_syscall1(int, sem_post, sem_t  *, sem) /* 唤醒在信号量上等待的进程，将信号量值加1；对应V原语 */
_syscall1(int, sem_unlink, const char  *, name)

int main (){
    const int NUM = 500;//生产总量
    const int p_NUM = 5;
    FILE * fp;//指向共享缓存区
    int buf[10],i,j=0,k=1;
    int n,out;
    sem_t *full, *empty, *mutex;
    full = sem_open("full",0); /* 产品剩余信号量，大于0则可消费 */
    empty = sem_open("empty",10);/* 空信号量，它与产品剩余信号量此消彼长，大于0时生产者才能继续生产 */
    mutex = sem_open("mutex",1); /* 互斥信号量，防止生产消费同时进行 */
/*fork（）函数通过系统调用创建一个与原来进程几乎完全相同的进程，

也就是两个进程可以做完全相同的事，但如果初始参数或者传入的变量不同，两个进程也可以做不同的事。

fork调用的一个奇妙之处就是它仅仅被调用一次，却能够返回两次，它可能有三种不同的返回值：
    1）在父进程中，fork返回新创建子进程的进程ID；
    2）在子进程中，fork返回0；
    3）如果出现错误，fork返回一个负值；*/
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
                /*读走全部的缓冲区，没被写入数字的就用-1初始化*/
                while(j<10){
                    out = -1;
		                fread(&out,sizeof(int),1,fp);
	                  buf[j] = out;
                    j++;
                }
		        fclose(fp);
		//getpid()获得当前的进程码
		        printf (" %d :%d ",getpid(),buf[0]);
		        fflush(stdout);
		        fp = fopen("buffer.txt","wb");
		/*下面的操作只回写其中的9个数据，以达到覆盖第一个的目的*/
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
