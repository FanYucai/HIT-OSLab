#include <string.h>
#include <errno.h>
#include <asm/segment.h>

char msg[24]; //23个字符 +'\0' = 24

int sys_iam(const char * name)

{
    //printk("im in iam");
        int i;
            char tmp[30];
                for(i=0; i<30; i++)
                {
                            tmp[i] = get_fs_byte(name+i);
                                    if(tmp[i] == '\0') break;  //字符串结束

                }
                    i=0;
                        while(i<30&&tmp[i]!='\0') i++;
                            int len = i;

                                if(len > 23)
                                {
                                            return -(EINVAL);

                                }
                                    strcpy(msg,tmp);

                                        return i;

}

int sys_whoami(char* name, unsigned int size)
{
    //printk("im in whoami");
    int len = 0;
    for(;msg[len]!='\0';len++);
    if(len > size)
    {
                return -(EINVAL);

    }
    int i = 0;
    for(i=0; i<size; i++)
    {
                put_fs_byte(msg[i],name+i);
                        if(msg[i] == '\0') break; //字符串结束

    }
    return i;
}
