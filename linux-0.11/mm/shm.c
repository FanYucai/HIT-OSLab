#pragma once
#define __LIBRARY__
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <asm/system.h>
#include <string.h>
#include <sys/types.h>

#define ENOMEM          12
#define EINVAL          22

int addr[20] = {0};

int sys_shmget(key_t key, size_t size){
    int free_page;
    if(addr[key] != 0)
    	return addr[key];
    else{  
        if(size > 1024*4)
        	return -EINVAL;
        if(!(free_page = get_free_page()))
        	return -ENOMEM;   
        else
        	addr[key] = free_page; 
        return addr[key];
    }       
}

void* sys_shmat(int shmid, const void *shmaddr){
    if(!shmid) 
    	return -EINVAL;
    put_page(shmid, current->start_code + current->brk);
    return current->brk;
}