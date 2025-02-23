#include <csos/syscall.h>
#include <logf.h>
int test_lds_num =123;

void init_entry()
{
    typedef void (*logfhandle)(const char * fmt, ...);
    logfhandle logfh = logf;
    test_lds_num++;
    char *argv[] = {"arg1", "arg2", NULL};
    // execve("/shell.elf", argv, NULL);
    int dd=2;
    while (1)
    {
       logfh("default task: %d\n", dd);
       dd++;
    //    return;
        sleep(2000);
    }
}