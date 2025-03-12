#include <kernel.h>
#include <tty.h>
#include <interrupt.h>
#include <logf.h>
#include <csos/time.h>
#include <task.h>
#include <csos/sem.h>
#include <csos/memory.h>
#include <device.h>
#include <disk.h>
#include <paging.h>
static memory_info_t g_memory_info;

uint32_t isstart = 0;
uint32_t int_count = 0;
uint32_t int_count0 = 0;
uint32_t count_rtc = 0;
typedef unsigned long long int uint64_t;
// static uint8_t g_stack[1024*10];
void csos_init(memory_info_t *mem_info, uint32_t gdt_info)
{
    g_memory_info = *mem_info;
    mem_info = &g_memory_info;
    gdt_info = 0; // unuse

    // 初始化中断
    interrupt_init();
    // 初始化调试输出
    logf_init();
    // 打开所需设备
    device_open(DEV_TTY, 0, NULL);
    // 初始化时间
    time_init(OS_TZ);
    // 初始化内存
    memory_init(mem_info);
    // GDT重载
    // gdt32_init((gdt_table_t*)gdt_info);
    gdt32_init();

    __asm__ volatile(
        // loader没有设置栈选择子,这里临时处理设置一下
        "push %eax\nmovw $0x10,%ax\nmovw %ax,%ss\npop %eax\n" // cli\n"
    );

    extern uint32_t keydown;
    if (0)
    {
        int GetCPUTime();
        uint8_t cmos_read(uint8_t addr);
        uint8_t sec = cmos_read(0);
        uint8_t sec2;
        tm time;
        while (1)
        {
            time_read(&time, OS_TZ);
            // sec2 =     cmos_read(0);
            if (time.tm_sec % 10 == 0)
            {
                GetCPUTime();
            }
        }
    }
    uint32_t count_rtc0 = count_rtc;
    uint32_t count_rtc3 = 0;
    void rtcinit();
    rtcinit();
    // sti();
    tm time1, time2;
    
extern uint32_t isNeedSW;
isNeedSW = 0;
time_read(&time1, OS_TZ);
tm time = time1;
logf("xxxxx STARTUP TIME: %d%d-%02d-%02d %02d:%02d:%02d",
     0,
     time.tm_year,
     time.tm_mon,
     time.tm_mday,
     time.tm_hour,
     time.tm_min,
     time.tm_sec);
extern uint32_t isRead;
isRead = 0;
extern uint32_t int_count;
extern uint32_t count_rtc;
int_count = 0;
count_rtc = 0;

if (0)
{
    while (1)
    {
        // if (isstart == 1)
        // {
        //     isstart = 2;
        //     time_read(&time1, OS_TZ);
        // }
       HLT;
        HLT;
        HLT;
        HLT;
        HLT;
        // HLT;
        // count_rtc3 = count_rtc;

        // if (count_rtc3 == 0)
        // {
        //     isstart = 1;
        // }

        if (0)
        {
            if (count_rtc0 + 1 == count_rtc3)
            {
                count_rtc0 = count_rtc3;
                void time_log_rtc();
                int_count0 = int_count;
                int_count = 0;
                time_log_rtc();
            }
            else if (count_rtc0 != count_rtc3)
            {
                logf("error:%u\t%u\n", count_rtc0, count_rtc3);
                count_rtc0 = count_rtc3;
            }
        }
        if (keydown || isstart == 3)
        {
            logf("count:%u\n", count_rtc3);
            void printw_rtc();
            printw_rtc();
            break;
            time_read(&time2, OS_TZ);
            keydown = 0;
            time = time1;
            logf("STARTUP TIME: %d%d-%02d-%02d %02d:%02d:%02d",
                 0,
                 time.tm_year,
                 time.tm_mon,
                 time.tm_mday,
                 time.tm_hour,
                 time.tm_min,
                 time.tm_sec);
            time = time2;
            logf("xxxEND TIME: %d%d-%02d-%02d %02d:%02d:%02d\tcost:%d",
                 0,
                 time.tm_year,
                 time.tm_mon,
                 time.tm_mday,
                 time.tm_hour,
                 time.tm_min,
                 time.tm_sec, (time.tm_min - time1.tm_min) * 60u + time.tm_sec - time1.tm_sec);
            // printw_rtc();
            break;
        }
        /* code */
    }
    }
    while (0)
    {
        HLT;
        HLT;
        HLT;
        HLT;
        HLT;
        HLT;
        HLT;
        HLT;
        HLT;
        /* code */
        if (keydown)
        {
            keydown = 0;

            void printw();
            printw();
            break;
        }
    }
    while ((0))
    {
        HLT;
    }

    // 检测磁盘
    // disk_init();
    // 初始化任务队列
    task_queue_init();
    // 初始化任务

    // default_task_init();
    // 开启中断
    // sti();
    // 打印信息
    logf("KL Version: %s; OS Version: %s", KERNEL_VERSION, OP_SYS_VERSION);
    // default任务
    task_t *task = get_running_task();
    // set_pde(0x10b000);

    tty_printf("qweqweqwe:%d\n", 1);
    void _rdmsr();
    _rdmsr();
    uint32_t *p7c = (uint32_t *)0x7c00;
    *p7c = 0x56781221;
    void _start_multi_core();
            asm __volatile__("sti");
    _start_multi_core();
    while (1)
    {
        int dd = 1;
        dd++;
        // HLT;
        /* code */
    }
    
    task_goto(task);
}

void a()
{
    static uint32_t child_task_stack2[1024];
    extern void main_task_entry2();
    extern void child_task_entry2();
    extern tss_task_t main_task2, child_task2;
    extern void tss_task_init2(tss_task_t * task, uint32_t entry, uint32_t esp);
    tss_task_init2(&child_task2, (uint32_t)child_task_entry2, (uint32_t)&child_task_stack2[1024]);
    tss_task_init2(&main_task2, 0, 0);
    write_tr(main_task2.selector);

    main_task_entry2();

    while (1)
    {
        /* code */
    }
}

inline uint64_t GetCount2() __attribute__((always_inline));
inline uint64_t GetCount2()
{
    // __asm__ volatile (
    // 	"rdtsc\n");
    uint32_t low, high;
    __asm__ volatile("rdtsc\n" : "=a"(low), "=d"(high));
    return (uint64_t)high << 32 | low;
}

typedef uint64_t u64;
#define do_div(n, base) ({        \
    unsigned int __base = (base); \
    unsigned int __rem;           \
    __rem = ((u64)(n)) % __base;  \
    (n) = ((u64)(n)) / __base;    \
    __rem;                        \
})

uint32_t doviv(uint64_t a, uint64_t base)
{
    uint64_t n = base;
    uint32_t i;

    for (i = 0; a > n; ++i)
    {
        a -= n;
    }
    return i;
}

uint32_t dom(uint64_t a, uint64_t n)
{
    return (a & (n - 1));
}

// 获取CPU时钟频率
int GetCPUTime()
{
    uint64_t start, end;
    uint32_t ticks = 0, ticksextra = 0;

    tm time;
    time_read(&time, OS_TZ);
    logf("start hh:%d:%d:%d\n", time.tm_hour, time.tm_min, time.tm_sec);
    start = GetCount2();
    end = GetCount2() - start;
    for (int i = 0; i < 10; i++)
    {
        start = GetCount2();
        end = GetCount2() - start;
        logf("cost:%d\t%u,%u\n", (uint32_t)end, (uint32_t)((start >> 32) & 0xFFFFFFFF), (uint32_t)((start & 0xFFFFFFFF)));
        /* code */
    }
    start = GetCount2();
    end = GetCount2() - start;
    start = GetCount2();

    // logf("%ld.%ldMHZ\n", ticks, ticksextra);
    // Sleep(1000);
    uint8_t cmos_read(uint8_t addr);
    uint8_t sec = cmos_read(0);
    uint8_t sec2;
    int t2 = time.tm_sec;
    while (1)
    {
        // time_read(&time, OS_TZ);
        sec2 = cmos_read(0);
        // if(time.tm_sec>t2)
        if (sec != sec2)
        {
            break;
        }
    }

    // uint64_t x=GetCount2();
    uint64_t y = GetCount2() - start - end;
    time_read(&time, OS_TZ);
    logf("end hh:%d:%d:%d\n", time.tm_hour, time.tm_min, time.tm_sec);
    // y=(uint64_t)y/(uint32_t)(1000u);
    uint32_t b = 1000u;
    //  uint32_t rem = do_div(y,b);
    ({
        unsigned int __base = (b);
        unsigned int __rem;
        __rem = dom(((u64)(y)), __base);
        (y) = doviv(((u64)(y)), __base);
    });
    // y = (uint32_t)y / 1000u;
    uint32_t cpuSpeed = (uint32_t)y; //( (x- start - end) / 1000 );
    ticks = cpuSpeed / 1000;
    ticksextra = cpuSpeed - (ticks * 100);

    logf("%d.%dMHZ\tt:%d\tsec:%d\tsec2:%d\n", ticks, ticksextra, t2, sec, sec2);
    // strCPUTime.fo("%ld.%ldMHZ", ticks, ticksextra);
    return 0;
}