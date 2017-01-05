#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define HZ      100


/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
    struct tms start_time, current_time;
    clock_t utime, stime;
    int sleep_time;

    while (last > 0)
    {
        /* CPU Burst */
        times(&start_time);
        /* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
         * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
         * 加上很合理。*/
        do
        {
            times(&current_time);
            utime = current_time.tms_utime - start_time.tms_utime;
            stime = current_time.tms_stime - start_time.tms_stime;
        } while ( ( (utime + stime) / HZ )  < cpu_time );
        last -= cpu_time;

        if (last <= 0 )
            break;

        /* IO Burst */
        /* 用sleep(1)模拟1秒钟的I/O操作 */
        sleep_time=0;
        while (sleep_time < io_time)
        {
            sleep(1);
            sleep_time++;
        }
        last -= sleep_time;
    }
}

  
int main(){
pid_t p1;
pid_t p2;
pid_t p3;
pid_t p4;
pid_t p5;
pid_t p6;

if((p1 = fork())==0 ){
	printf("Process 1\n");
	cpuio_bound(10,10,0);
	printf("Process 1 finished\n");
}
else if((p2 = fork())==0){
	printf("Process 2\n");
	cpuio_bound(10,2,8);
	printf("Process 2 finished\n");
}
else if((p3 = fork())==0){
	printf("Process 3\n");
	cpuio_bound(10,4,6);
	printf("Process 3 finished\n");
}
else if((p4 = fork())==0){
	printf("Process 4\n");
	cpuio_bound(10,6,4);
	printf("Process 4 finished\n");
}
else if((p5 = fork())==0){
	printf("Process 5\n");
	cpuio_bound(10,8,2);
	printf("Process 5 finished\n");
}
else if((p6 = fork())==0){
	printf("Process 6\n");
	cpuio_bound(10,10,0);
	printf("Process 6 finished\n");
}
else if(p1==-1||p2==-1||p3==-1||p4==-1||p5==-1||p6==-1){
	perror("fork");
	exit(1);
}
else{
	printf("Parent's pid is %d\n",getpid());
	printf("The pid of Process 1 is %d\n",p1);
	printf("The pid of Process 2 is %d\n",p2);
	printf("The pid of Process 3 is %d\n",p3);
	printf("The pid of Process 4 is %d\n",p4);
	printf("The pid of Process 5 is %d\n",p5);
	printf("The pid of Process 6 is %d\n",p6);
}
wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);
return 0;
}
