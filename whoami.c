nclude <errno.h>
#define __LIBRARY__
#include <unistd.h>

_syscall2(int, whoami,char*,name,unsigned int,size);

int main()
{
        char s[30] = "I dont know who you are";
            whoami(s,30);
                printf("%s",s);
                    return 0;

}
