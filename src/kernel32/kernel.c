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

static memory_info_t g_memory_info;

// static uint8_t g_stack[1024*10];
void csos_init(memory_info_t* mem_info, uint32_t gdt_info)
{
    g_memory_info = * mem_info;
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
        "push %eax\nmovw $0x10,%ax\nmovw %ax,%ss\npop %eax\n"//cli\n"
     );

    // 检测磁盘
    // disk_init();
    // 初始化任务队列
    task_queue_init();
    // 初始化任务
    default_task_init();
    // 开启中断
    // sti();
    // 打印信息
    logf("KL Version: %s; OS Version: %s", KERNEL_VERSION, OP_SYS_VERSION);
    // default任务
    task_t *task = get_running_task();
    task_goto(task);
}

void a(){
    static uint32_t child_task_stack2[1024];
    extern void main_task_entry2();
    extern void child_task_entry2();
    extern tss_task_t main_task2, child_task2;
    extern void tss_task_init2(tss_task_t *task, uint32_t entry, uint32_t esp);
    tss_task_init2(&child_task2, (uint32_t)child_task_entry2, (uint32_t)&child_task_stack2[1024]);
    tss_task_init2(&main_task2, 0, 0);
    write_tr(main_task2.selector);

    main_task_entry2();

    while (1)
    {
        /* code */
    }
    
}