#define __LIBRARY__
#include <unistd.h>
_syscall1(int, iam, const char*, name);
_syscall2(int, whoami,char*,name,unsigned int,size);
