#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <asm/system.h>
#include <string.h>

extern void sleep_on_queue();
extern void wake_up_queue(struct task_struct **p);
extern int mystrcmp(const char* str1,const char* str2);

char* sem_name[64]={};//用于存储全部的信号量的名字
sem_t sem[64]={};//用于存储全部的信号量

char name_array[64][128]={};//临时存放名字
Que wait_Q[64];
int flag[64]={0};
/*初始化环形队列*/
void initQue(Que* Q)
{
    int i;
    Q->front = 0;
    Q->rear = QUEUE_SIZE-1;
    for(i=0;i<QUEUE_SIZE;i++)
        Q->waitQue[i] = NULL;
}
/*判断队列是否为空*/
int emptyQue(Que Q)
{
    if(((Q.rear+1)%QUEUE_SIZE) == Q.front)
        return 1;
    else
        return 0;
}
/*判断队列是否填满*/
 fullQue(Que Q)
{
    if(((Q.rear+2)%QUEUE_SIZE) == Q.front)
	      return 1;
    else
        return 0;
}
/*取队列头*/
struct task_struct* getfront(Que Q)
{
    if(emptyQue(Q))
        return NULL;
    else
        return (Q.waitQue[Q.front]);
}
/*入队*/
void enqueue(struct task_struct* p,Que* Q)
{
    if(fullQue(*Q)) _exit(1);
    else
    {
        Q->rear = (Q->rear+1)%QUEUE_SIZE;
        Q->waitQue[Q->rear] = p;
    }
}
/*出队*/
void dequeue(Que* Q)
{
    if(!emptyQue(*Q))
        Q->front = (Q->front+1)%QUEUE_SIZE;
    else
        _exit(1);
}
/*将信号量名字写入内核保存*/
char * CreatSem(const char* name)
{
    int i=0;
    char *temp = malloc(sizeof(char)*64);
    while((temp[i] = get_fs_byte(name+i)) != '\0')
	i++;
    return temp;
}
/*比较两个字符串是否相等*/
int mystrcmp(const char* str1,const char* str2)
{
    int i=0,j=0;
    if(str1 == NULL || str2 == NULL)
	return 0;
    while((*(str1+i)) != '\0')
    {
        if((*(str1+i)) != (*(str2+j)))
	    return 0;
	i++;
	j++;
    }
    if((*(str2+j)) == '\0')
	return 1;
    else
	return 0;
}

int judgeSem(char * name)
{
    int i=0;
    if(get_fs_byte(name) == '\0')
	return -1;
    else
    {
        while(i<64)
        {
            if (mystrcmp(sem_name[i],name))
                return i;
            i++;
        }
        return -1;
    }
}
/*打开或者新建一个信号量*/
sem_t *sys_sem_open(const char *name, unsigned int value)
{
    int lable;
    int i=0;
    sem_t * n = NULL;
    char* tname = CreatSem(name);
    lable = judgeSem(tname);/*信号量存在就返回指针*/
    if(lable>=0)/*返回指针*/
        return (&sem[lable]);
    else /*信号量不存在则创建新的信号量*/
    {
        while(i<64)
        {
            if(flag[i]!=1)
            {
                if(name_array[i][0] == '\0')
                    sem_name[i] = strcpy(name_array[i],tname);
                sem[i].value = value;
                sem[i].queue = &wait_Q[i];
	        initQue(sem[i].queue);
                flag[i]=1;
	        return &sem[i];
           }
           i++;
        }
    }
    return n;
}
/*清除一个信号量*/
int sys_sem_unlink(const char *name)
{
    char* tname;
    int lable;
    int i=0;
    if(get_fs_byte(name) == '\0')
        return -1;
    tname = CreatSem(name);
    lable = judgeSem(tname);
    if(lable>=0)
    {
        flag[lable]=0;
        sem_name[lable] = NULL;
        sem[lable].value = 0;
        sem[lable].queue = NULL;
        for(i=0;i<128;i++)
            name_array[lable][i] ='\0';
        return 1;
   }
   else return 1;
}
/*wait操作*/ /* 等待信号量至其值大于0，将其值减1；对应P原语 */
int sys_sem_wait(sem_t* sema)
{
    if(sema == NULL)
	return -1;
    cli();//关中断
    sema->value--;
    if((sema->value) < 0)//小于0 进入等待队列
    {
        enqueue(current,sema->queue);
        current->state = TASK_UNINTERRUPTIBLE;//进程主动睡眠
       	schedule();
    }
    sti();//开中断
    return 0;
}
/*post操作*/ /* 唤醒在信号量上等待的进程，将信号量值加1；对应V原语 */
int sys_sem_post(sem_t* sema)
{
    if(sema == NULL)
	      return -1;
    cli();//关中断
    sema->value++;
    if((sema->value) <= 0)
    {
	      struct task_struct* front;
	      front = getfront(*(sema->queue));
	      dequeue(sema->queue);//将队首去掉
	      wake_up_queue(&front);//唤醒等待信号
    }
    sti();//开中断
    return 0;
}
void wake_up_queue(struct task_struct **p)
{
	if(p&& *p)
	{
		(**p).state = TASK_RUNNING;
	}
}
