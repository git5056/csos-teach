#include <task/tss.h>
#include <csos/string.h>
#include <csos/memory.h>
#include <csos/syscall.h>
#include <csos/stdlib.h>
#include <paging.h>
#include <interrupt.h>
#include <logf.h>

tss_task_t main_task2, child_task2;

static inline void far_jump2(uint32_t selector, uint32_t offset) {
	uint32_t addr[] = { offset, selector };
	__asm__ volatile("ljmpl *(%[a])"::[a]"r"(addr));
}


void tss_task_switch2(tss_task_t *from, tss_task_t *to)
{
    far_jump2(to->selector, 0);
}


void child_task_entry2()
{
    int counter = 0;
    counter++;
    while (TRUE) {
        logf("child task: %d", counter++);
        counter++;
        tss_task_switch2(&child_task2, &main_task2);
        // simple_task_switch(&child_task, &main_task);
    }
}

void main_task_entry2()
{
    int counter = 0;
    while (TRUE) {
        logf("main task: %d", counter++);
        tss_task_switch2(&main_task2, &child_task2);
        // simple_task_switch(&main_task, &child_task);
    }
}

static void tss_init2(tss_task_t *task, uint32_t entry, uint32_t esp)
{
    uint32_t selector = alloc_gdt_table_entry();
    if (selector < 0) return;

    tss_t *tss = &task->tss;
    set_gdt_table_entry(selector, (uint32_t)tss, sizeof(tss_t),
        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_TYPE_TSS);

    kernel_memset(tss, 0, sizeof(tss_t));
    tss->eip = entry;
    tss->esp = tss->esp0 = esp;
    tss->es = tss->ds =  tss->fs =  tss->gs = tss->ss = tss->ss0 = KERNEL_DATA_SEG;
    tss->cs = KERNEL_CODE_SEG;
    tss->eflags = EFLAGS_DEFAULT ;//| EFLAGS_IF;
    static int num =0;
    if(num!=0){
        // tss->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    }
    num++;
    task->selector = selector;
}

void tss_task_init2(tss_task_t *task, uint32_t entry, uint32_t esp)
{
    tss_init2(task, entry, esp);
}