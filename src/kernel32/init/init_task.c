#include <csos/syscall.h>
#include <logf.h>
#include <task.h>
int test_lds_num = 123;
void psleep(uint32_t ms);

void init_entry()
{
    typedef void (*logfhandle)(const char *fmt, ...);
    typedef int (*printfHandle)(const char *fmt, ...);
    typedef void (*tss_task_sleephandle)(uint32_t ms);
    typedef void (*psleephandle)(uint32_t ms);
    typedef void (*sleephanlde)(uint32_t ms);
    int printf2(const char *fmt, ...);
    logfhandle logfh = logf;
    printfHandle printh = printf;
    sleephanlde sleeph = sleep;
    psleephandle psleeph = psleep;
    printfHandle printf2h = printf2;

    tss_task_sleephandle _tss_task_sleephandle = task_sleep;
    test_lds_num++;
    char *argv[] = {"arg1", "arg2", NULL};
    // execve("/shell.elf", argv, NULL);
    // execve
    int dd = 2;
    int *p = &dd;
    while (1)
    {
        for (int i = 0; i < 100; i++)
        {
            *p = *p + 1;
            /* code */
        }

        // printf("default task: %d\n", dd);
        printf2h("default task: %d\n", dd);
        //    logfh("default task: %d\n", dd);
        dd++;
        //    return;
        // sleep(2000);
        psleeph(2000);
        // sleeph(2000);
        // _tss_task_sleephandle(2000);
    }
}