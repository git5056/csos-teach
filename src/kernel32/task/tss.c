

#include <task/tss.h>
#include <csos/string.h>
#include <csos/memory.h>
#include <csos/syscall.h>
#include <csos/stdlib.h>
#include <paging.h>
#include <interrupt.h>
#include <logf.h>

static mutex_t task_mutex;
static tss_task_t task_table[OS_TASK_MAX_SIZE];

tss_task_queue_t tss_task_queue;
static uint32_t task_pid = 0;

int funczza()
{
    int dd = 2;
    int cc = 123;
    for (int i = 0; i < dd; i++)
    {
        cc += cc;
        cc *= 123;

        /* code */
    }

    return cc;
}

static uint32_t idle_task_stack[1024];
static void idle_task_entry()
{
    int d = 1;
    int *p = &d;
    int zxc = 0;
    // while (TRUE) HLT;
    while (TRUE)
    {
        // *p++;
        // for (int i = 0; i < *p; i++)
        // {
        //     zxc = funczza(); /* code */
        // }
        
        // *p = 1;

        // int dd=2;
        // dd++;
        HLT;
    };
}

static int tss_init(tss_task_t *task, uint32_t flag, uint32_t entry, uint32_t esp)
{
    uint32_t selector = alloc_gdt_table_entry();
    if (selector < 0)
        return selector;

    const uint32_t a = sizeof(tss_t);
    tss_t *tss = &task->tss;
    set_gdt_table_entry(selector, (uint32_t)tss, sizeof(tss_t),
                        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_TYPE_TSS);

    kernel_memset(tss, 0, sizeof(tss_t));

    uint32_t kernel_stack = alloc_page();

    uint32_t uc_selector = KERNEL_CODE_SEG, ud_selector = KERNEL_DATA_SEG;
    if (flag & TASK_LEVEL_USER)
    {
        uc_selector = tss_task_queue.uc_selector | SEG_ATTR_CPL3;
        ud_selector = tss_task_queue.ud_selector | SEG_ATTR_CPL3;
    }

    tss->eip = entry;
    tss->esp = esp;
    tss->esp0 = kernel_stack + PAGE_SIZE;
    tss->ss = tss->es = tss->ds = tss->fs = tss->gs = ud_selector;
    tss->ss0 = KERNEL_DATA_SEG;
    tss->cs = uc_selector;
    tss->eflags = EFLAGS_DEFAULT | EFLAGS_IF;

    uint32_t pde = memory32_create_pde();
    if (pde == 0)
    {
        free_gdt_table_entry(selector);
        return -1;
    }
    tss->cr3 = pde;
    task->selector = selector;
    return 0;
}

static task_t *alloc_task()
{
    tss_task_t *task = NULL;
    mutex_lock(&task_mutex);
    for (int i = 0; i < OS_TASK_MAX_SIZE; i++)
    {
        tss_task_t *t = &task_table[i];
        if (t->name[0] == 0)
        {
            task = t;
            break;
        }
    }
    mutex_unlock(&task_mutex);
    return task;
}

uint32_t tss_task_getpid()
{
    task_t *task = get_running_task();
    return task->pid;
}

static uint32_t default_task_stack2[1024];
void default_task_entry2()
{
    int dd = 2;
    // while (TRUE) HLT;
    while (TRUE)
    {
        dd++;
        // printf("default_task_entry2:%d\n",dd);
        // logf("default_task_entry2:%d\n",dd);
        // return;

        sleep(1000);
        // task_sleep(1000);
    };
}

static uint8_t sta[4096] __attribute__((aligned(PAGE_SIZE)));
void tss_task_queue_init()
{
    kernel_memset(task_table, 0, sizeof(task_table));
    mutex_init(&task_mutex);

    uint32_t ud_selector = alloc_gdt_table_entry();
    set_gdt_table_entry(ud_selector, 0x0, 0xFFFFFFFF,
                        SEG_ATTR_P | SEG_ATTR_DPL3 | SEG_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_ATTR_D);
    tss_task_queue.ud_selector = ud_selector;

    uint32_t uc_selector = alloc_gdt_table_entry();
    set_gdt_table_entry(uc_selector, 0x0, 0xFFFFFFFF,
                        SEG_ATTR_P | SEG_ATTR_DPL3 | SEG_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_ATTR_D);
    tss_task_queue.uc_selector = uc_selector;

    list_init(&tss_task_queue.ready_list);
    list_init(&tss_task_queue.task_list);
    list_init(&tss_task_queue.sleep_list);
    tss_task_queue.running_task = NULL;
    // 初始化空闲任务
    tss_task_init(&tss_task_queue.idle_task, "idle task", TASK_LEVEL_SYSTEM, (uint32_t)idle_task_entry, (uint32_t)&idle_task_stack[1024]);

    extern void xxxfunc();
#define qqdzasdzxc (0x41000000)
    tss_task_init(&tss_task_queue.default_task2, "default task2", TASK_LEVEL_USER,
                  //  (uint32_t)xxxfunc ,
                  qqdzasdzxc,
                  qqdzasdzxc + 0x500);
    kernel_memcpy((void *)&sta, (void *)xxxfunc, 100);
    uint32_t wpde = tss_task_queue.default_task2.tss.cr3;
    int memory32_create_map(pde_t * pde, uint32_t vaddr, uint32_t paddr, uint32_t count, uint32_t perm);
    memory32_create_map((pde_t *)wpde, qqdzasdzxc, (uint32_t)&sta, 1, PTE_P | PTE_W | PTE_U);
    // set_pde(wpde);
    int dd = 2;
    dd++;
    dd++;
}

void default_tss_task_init()
{
    // default task 入口
    void init_task_entry();
    // default task 代码开始结束位置
    extern uint8_t b_init_task[], e_init_task[];
    // 计算需要拷贝的字节数
    uint32_t xxe = (uint32_t)e_init_task;
    uint32_t l_b_init_task = (uint32_t)b_init_task;
    uint32_t l_e_init_task = (uint32_t)e_init_task;
    void *pxxe = &e_init_task;
    void *pxxb = &b_init_task;
    uint32_t copy_size = (uint32_t)(e_init_task - b_init_task);
    // 分配空间
    uint32_t alloc_size = PAGE_SIZE * 10;
    // 初始化任务
    uint32_t init_start = (uint32_t)init_task_entry;
    uint32_t v_init_start = init_start;
    extern uint32_t gadd;
    uint32_t l_init_start = v_init_start + gadd; // 注释1. laddr,由于这里lds里面设置了noload,因此init_task段是没有重定向的,因此这里直接使用laddr为新的pde下的虚拟地址
    // 注释2,注释1的方案已废弃.已取消(NOLOAD),因此现在需要手动重定向一下地址,重定向数据存放在499扇区
    init_start = 0x51000000;
    uint32_t v_b_init_task = l_b_init_task - gadd; // vaddr
    void *pb_init_task = (void *)b_init_task;
    // tss_task_init(&tss_task_queue.default_task, "default task", TASK_LEVEL_USER, init_start, init_start + alloc_size);
    // tss_task_init(&tss_task_queue.default_task, "default task", TASK_LEVEL_USER, l_init_start, l_b_init_task + alloc_size); // 见注释1
    tss_task_init(&tss_task_queue.default_task, "default task", TASK_LEVEL_USER, l_init_start, l_b_init_task + alloc_size); // 见注释1

    tss_task_queue.default_task.bheap = (uint32_t)e_init_task;
    tss_task_queue.default_task.eheap = (uint32_t)e_init_task;
    write_tr(tss_task_queue.default_task.selector);
    tss_task_queue.running_task = &tss_task_queue.default_task;
    uint32_t pde = tss_task_queue.default_task.tss.cr3;
    // set_pde(pde);
    //  alloc_pages(pde, init_start, alloc_size, PTE_P | PTE_W | PTE_U);
    alloc_pages(pde, l_b_init_task, alloc_size, PTE_P | PTE_W | PTE_U); // 加载地址就是新的pde对应的虚拟地址,为这个地址映射内存
    // kernel_memcpy((void *)init_start, (void *)b_init_task, copy_size);
    uint32_t xxx = (uint32_t)b_init_task;
    set_pde(pde);
    extern int aaab;
    // aaab=1;
    kernel_memcpy((void *)l_b_init_task, (void *)v_b_init_task, copy_size); // 由于系统pde和新建的pde的低位内存存在相同的映射,故可以直接复制
    // aaab=0;
    // *(uint32_t*)(v_b_init_task+0x20)=0x12345678;
    // 见注释2
    void relocation_init_task(void *dst, uint32_t l_b_init_task, uint32_t l_e_init_task);
    relocation_init_task((void *)l_b_init_task, l_b_init_task, l_e_init_task);
    // kernel_memcpy((void *)0x10c000, (void *)0x60000, copy_size);
    // set_pde(pde);
}

#define is_hex_digit(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f'))

static inline uint32_t skip_atoi_hex(const char **s)
{
    uint32_t i = 0;

    while (is_hex_digit(**s))
    {
        if (**s >= 'a')
        {
            i = i * 16u + *((*s)++) - 'a' + 10;
        }
        else
        {
            i = i * 16u + *((*s)++) - '0';
        }
    }

    return i;
}

void relocation_init_task(void *dst, uint32_t l_b_init_task, uint32_t l_e_init_task)
{
    uint8_t buffer[512];
    read_disk(499, 1, (uint16_t *)buffer);
    uint8_t *src, *begin, c;
    begin = src = buffer;
    uint32_t relocations[100], relocations_len = 0;
    while ((c = *src++) != '\0')
    {
    };
    src--;
    *src++ = '\n';
    *src++ = '\0';
    src = buffer;
    while ((c = *src++) != '\0')
    {
        if (c == '\r' || c == '\n')
        {
            if (begin != 0)
            {
                uint32_t num = skip_atoi_hex((const char **)&begin);
                relocations[relocations_len++] = num;
                num++;
            }

            begin = 0;
        }
        else if (begin == 0)
        {
            begin = src - 1;
        }
    }
    extern uint32_t gadd;
    uint32_t imagebase = 0x400000, foffset, taddr, laddr, vaddr;
    uint32_t v_b_init_task = l_b_init_task - gadd;
    uint32_t v_e_init_task = l_e_init_task - gadd;
    uint32_t tadd1 = (uint32_t)dst - v_b_init_task;
    for (uint32_t i = 0; i < relocations_len; i++)
    {
        logf("relocation addr:%x\n", relocations[i]);
        laddr = imagebase + relocations[i];
        vaddr = laddr - gadd;
        foffset = laddr - l_b_init_task;
        taddr = *(uint32_t *)(dst + foffset);
        if (taddr >= v_b_init_task && taddr <= v_e_init_task)
        {
            void *asd = (void *)(taddr + tadd1);
            logf("xxx addr:%x\t==>%x\n", foffset, taddr + tadd1);
            *(uint32_t *)(dst + foffset) = taddr + tadd1;
        }
    }
}

tss_task_t *get_default_tss_task()
{
    return &tss_task_queue.default_task;
}

tss_task_t *get_running_tss_task()
{
    return tss_task_queue.running_task;
}

void tss_task_set_ready(tss_task_t *task)
{
    // 防止空闲任务进入运行任务队列
    if (task == &tss_task_queue.idle_task)
        return;

    task->state = TASK_READY;
    list_insert_back(&tss_task_queue.ready_list, &task->running_node);
}

void tss_task_set_block(tss_task_t *task)
{
    // 防止空闲任务进入运行任务队列
    if (task == &tss_task_queue.idle_task)
        return;

    list_remove(&tss_task_queue.ready_list, &task->running_node);
}

void tss_task_yield()
{
    if (tss_task_queue.ready_list.size > 1)
    {
        tss_task_t *task = get_running_tss_task();
        tss_task_set_block(task);
        tss_task_set_ready(task);
        tss_task_dispatch();
    }
}

int tss_task_fork()
{
    tss_task_t *parent = get_running_task();
    tss_task_t *child = alloc_task();
    if (child == NULL)
        return -1;

    syscall_frame_t *frame = (syscall_frame_t *)(parent->tss.esp0 - sizeof(syscall_frame_t));
    int ret = tss_task_init(child, parent->name, TASK_LEVEL_USER,
                            frame->eip,
                            frame->_esp + sizeof(uint32_t) * SYSCALL_PMC);
    if (ret != 0)
    {
        tss_task_destroy(child);
        return -1;
    }

    tss_t *tss = &child->tss;
    tss->eax = 0;
    tss->edx = frame->edx;
    tss->ecx = frame->ecx;
    tss->ebx = frame->ebx;
    tss->esi = frame->esi;
    tss->edi = frame->edi;
    tss->ebp = frame->ebp;
    tss->cs = frame->cs;
    tss->ds = frame->ds;
    tss->es = frame->es;
    tss->gs = frame->gs;
    tss->fs = frame->fs;
    tss->eflags = frame->eflags;

    child->parent = parent;

    if ((tss->cr3 = copy_page(parent->tss.cr3)) < 0)
    {
        tss_task_destroy(child);
        return -1;
    }

    return child->pid;
}

void tss_task_exit(int code)
{
    task_t *task = get_running_task();
    if (task->state == TASK_RUNNING)
    {
        task->state = TASK_DIED;
        task->exit_code = code;
    }
    list_remove(&tss_task_queue.ready_list, &task->running_node);
    list_remove(&tss_task_queue.task_list, &task->task_node);
    tss_task_destroy(task);
    tss_task_dispatch();
}

static uint8_t SHELL_TMP[20 * 512];

static uint32_t load_elf_file(task_t *task, const char *name, uint32_t pde)
{
    Elf32_Ehdr elf_hdr;
    Elf32_Phdr elf_phdr;
    read_disk(1000, 20, (uint16_t *)SHELL_TMP);
    uint8_t *buffer = SHELL_TMP;
    kernel_memcpy(&elf_hdr, buffer, sizeof(Elf32_Ehdr));
    buffer += sizeof(Elf32_Ehdr);
    if (elf_hdr.e_ident[0] != 0x7F || elf_hdr.e_ident[1] != 'E' || elf_hdr.e_ident[2] != 'L' || elf_hdr.e_ident[3] != 'F')
        return 0;
    uint32_t e_phoff = elf_hdr.e_phoff;
    for (int i = 0; i < elf_hdr.e_phnum; i++)
    {
        buffer = SHELL_TMP + e_phoff;
        kernel_memcpy(&elf_phdr, buffer, sizeof(Elf32_Phdr));
        buffer += sizeof(Elf32_Phdr);
        if ((elf_phdr.p_type != 1) || (elf_phdr.p_vaddr < VM_TASK_BASE))
            continue;
        int err = alloc_pages(pde, elf_phdr.p_vaddr, elf_phdr.p_memsz, PTE_P | PTE_U | PTE_W);
        if (err < 0)
            return -1;
        buffer = SHELL_TMP + elf_phdr.p_offset;
        uint32_t vaddr = elf_phdr.p_vaddr;
        uint32_t size = elf_phdr.p_filesz;
        while (size > 0)
        {
            int cs = (size > PAGE_SIZE) ? PAGE_SIZE : size;
            uint32_t paddr = memory32_get_paddr(pde, vaddr);
            kernel_memcpy((void *)paddr, buffer, cs);
            buffer += cs;
            size -= cs;
            vaddr += cs;
        }
        task->eheap = task->bheap = elf_phdr.p_vaddr + elf_phdr.p_memsz;
    }
    return elf_hdr.e_entry;
}

static int copy_args(uint32_t pde, char *dst, char *argv[], int argc)
{
    task_args_t task_args;
    task_args.argc = argc;
    task_args.argv = (char **)(dst + sizeof(task_args_t));

    char *dst_arg = dst + sizeof(task_args_t) + sizeof(char *) * argc;
    char **dst_arg_tb = (char **)memory32_get_paddr(pde, (uint32_t)(dst + sizeof(task_args_t)));
    for (int i = 0; i < argc; i++)
    {
        char *from = argv[i];
        int len = kernel_strlen(from) + 1;
        int err = memory32_copy_page_data((uint32_t)dst_arg, pde, (uint32_t)from, len);
        if (err < 0)
            return -1;
        dst_arg_tb[i] = dst_arg;
        dst_arg += len;
    }
    memory32_copy_page_data((uint32_t)dst, pde, (uint32_t)&task_args, sizeof(task_args));
}

int tss_task_execve(char *name, char *argv[], char *env[])
{
    tss_task_t *task = get_running_task();
    kernel_strncpy(task->name, get_file_name(name), TASK_NAME_SIZE);
    uint32_t old_pde = task->tss.cr3;
    uint32_t new_pde = memory32_create_pde();
    if (new_pde <= 0)
        return -1;
    uint32_t entry = load_elf_file(task, name, new_pde);
    if (entry == 0)
    {
        task->tss.cr3 = old_pde;
        set_pde(old_pde);
        destroy_page(old_pde);
        return -1;
    }
    uint32_t stack_top = VM_SHELL_STACK - VM_SHELL_ARGS_SIZE;
    if (alloc_pages(new_pde, VM_SHELL_STACK - VM_SHELL_STACK_SIZE, VM_SHELL_STACK_SIZE, PTE_U | PTE_W | PTE_P) < 0)
    {
        task->tss.cr3 = old_pde;
        set_pde(old_pde);
        destroy_page(old_pde);
        return -1;
    }
    int argc = strings_count(argv);
    if (copy_args(new_pde, (char *)stack_top, argv, argc) < 0)
    {
        task->tss.cr3 = old_pde;
        set_pde(old_pde);
        destroy_page(old_pde);
        return -1;
    }
    syscall_frame_t *frame = (syscall_frame_t *)(task->tss.esp0 - sizeof(syscall_frame_t));
    frame->eip = entry;
    frame->eax = frame->ebx = frame->ecx = frame->edx = 0;
    frame->edi = frame->esi = frame->ebp = 0;
    frame->eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    frame->_esp = stack_top - sizeof(uint32_t) * SYSCALL_PMC;

    task->tss.cr3 = new_pde;
    set_pde(new_pde);
    destroy_page(old_pde);
    return 0;
}

uint8_t *tss_task_sbrk(uint32_t size)
{
    tss_task_t *task = get_running_task();
    uint8_t *peheap = (uint8_t *)task->eheap;
    if (size == 0)
        return peheap;

    uint32_t start = task->eheap;
    uint32_t stop = start + size;
    int start_offset = start % PAGE_SIZE;
    if (start_offset)
    {
        if (start_offset + size <= PAGE_SIZE)
        {
            task->eheap = stop;
            return peheap;
        }
        else
        {
            uint32_t cs = PAGE_SIZE - start_offset;
            start += cs;
            size -= cs;
        }
    }

    if (size)
    {
        uint32_t cs = stop - start;
        if (alloc_pages(task->tss.cr3, start, cs, PTE_P | PTE_U | PTE_W) < 0)
        {
            return (uint8_t *)NULL;
        }
    }

    task->eheap = stop;
    return peheap;
}

void tss_task_destroy(tss_task_t *task)
{
    if (task->selector)
    {
        free_gdt_table_entry(task->selector);
    }

    if (task->tss.esp0)
    {
        free_page(task->tss.esp - PAGE_SIZE);
    }

    if (task->tss.cr3)
    {
        destroy_page(task->tss.cr3);
    }
    kernel_memset(task, 0, sizeof(task_t));
}

void call_cpuid()
{
    unsigned eax, ebx, ecx, edx;
    __asm__ __volatile__(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1) // 功能号为1，用于获取处理器的特征信息
    );
    logf("CPUID EAX: %x\n", eax);
    logf("CPUID EBX: %x\n", ebx);
    logf("CPUID ECX: %x\n", ecx);
    logf("CPUID EDX: %x\n", edx);
}

uint32_t isNeedSW = 0;
uint32_t reteipaddr = 0;

uint32_t switch_task()
{
    syscall_arg_t sleep_arg = {SYS_NR_SW, 0, 0, 0, 0};
    return (uint32_t)_syscall(&sleep_arg);
}



uint32_t switch_task2()
{
        protect_state_t ps = protect_enter();
    //   list_node_t *node = list_get_first(&tss_task_queue.sleep_list);
    //   while (node)
    //   {
    //       tss_task_t *task = struct_from_field(node, tss_task_t, running_node);
    //       if (-- task->sleep == 0) {
    //           tss_task_notify(task);
    //           tss_task_set_ready(task);
    //       }
    //       node = task->running_node.next;
    //   }

    // tss_task_yield();

    tss_task_t *task = get_running_tss_task();
    task->ticks = task->slices;
    tss_task_set_block(task);
    tss_task_set_ready(task);
    uint32_t _retipaddr = reteipaddr;
    isNeedSW = 0;
    tss_task_dispatch();
    // uint32_t _retipaddr = reteipaddr;
    // isNeedSW = 0;

    protect_exit(ps);
    int dd = 2;
    dd++;
    //   call_cpuid();
    return _retipaddr;
}

#include <time.h>

uint32_t ms_spent=0;
void simple_sleep(){
  uint32_t sleep_time = 20 + ms_spent;
        while (1) {
            if (ms_spent > sleep_time)
                break;
        }   
}

void tss_task_ts()
{
    ms_spent += (1000u/200u);
    return ;

    // return;

    // void time_init2();
    // extern uint32_t keydown;

    // if(keydown==0){

    // time_init2();
    // }
    // return;
    //  return;
    protect_state_t ps = protect_enter();
    // isNeedSW = 0;

    list_node_t *node = list_get_first(&tss_task_queue.sleep_list);
    while (node)
    {
        // tss_task_t *task = struct_from_field(node, tss_task_t, running_node);
        tss_task_t *task;
        struct_from_field(task, node, tss_task_t, running_node);
        if (--task->sleep == 0)
        {
            tss_task_notify(task);
            tss_task_set_ready(task);
        }
        node = task->running_node.next;
    }

    tss_task_t *task = get_running_tss_task();
    if (isNeedSW == 0)
    {
        if (--task->ticks == 0)
        {
            isNeedSW = 1;
            return;
        }

        // if (task == &tss_task_queue.idle_task && tss_task_queue.ready_list.size > 0){
        //     isNeedSW=1;
        // }
    }

    protect_exit(ps);
    return;

    // list_node_t *node = list_get_first(&tss_task_queue.sleep_list);
    while (node)
    {
        tss_task_t *task;
        struct_from_field(task, node, tss_task_t, running_node);
        if (--task->sleep == 0)
        {
            isNeedSW = 1;
            protect_exit(ps);
            return;
            // tss_task_notify(task);
            // tss_task_set_ready(task);
        }
        node = task->running_node.next;
    }

    // tss_task_t *task = get_running_tss_task();
    if (--task->ticks == 0)
    {
        task->ticks = task->slices;
        tss_task_set_block(task);
        tss_task_set_ready(task);
        tss_task_dispatch();
    }
    protect_exit(ps);
}

static void tss_task_set_sleep(tss_task_t *task, uint32_t ticks)
{
    if (ticks == 0)
        return;

    task->sleep = ticks / 5;
    task->state = TASK_SLEEP;
    list_insert_back(&tss_task_queue.sleep_list, &task->running_node);
}

void tss_task_sleep(uint32_t ms)
{
    tss_task_t *pt = tss_task_queue.running_task;
    tss_task_set_block(tss_task_queue.running_task);

    tss_task_set_sleep(tss_task_queue.running_task, ms);
    // todo,better check
    // tss_task_dispatch();
    // pt ==  tss_task_queue.running_task &&
    if (tss_task_queue.running_task->state == TASK_SLEEP)
    {
        tss_task_dispatch();
    }
}

void tss_task_notify(tss_task_t *task)
{
    list_remove(&tss_task_queue.sleep_list, &task->running_node);
}

void tss_task_switch(tss_task_t *from, tss_task_t *to, uint32_t cr3)
{
    far_jump23(to->selector, 0, cr3);
}

void tss_task_dispatch()
{
    protect_state_t ps = protect_enter();
    tss_task_t *to = &tss_task_queue.idle_task, *from = get_running_tss_task();
    // 判断运行队列是否为空，为空则运行空闲任务
    if (!list_is_empty(&tss_task_queue.ready_list))
    {
        list_node_t *node = list_get_first(&tss_task_queue.ready_list);
        // tss_task_t * to2 = struct_from_field(node, tss_task_t, running_node);
        tss_task_t *to2;
        struct_from_field(to2, node, tss_task_t, running_node);

        // todo??
        // if(to2!=from){
        to = to2;
        // }
    }

    if (to != from)
    {
        tss_task_queue.running_task = to;
        to->state = TASK_RUNNING;
        // set_pde(to->tss.cr3);
        tss_task_switch(from, to, to->tss.cr3);
    }
    protect_exit(ps);
}

int tss_task_init(tss_task_t *task, const char *name, uint32_t flag, uint32_t entry, uint32_t esp)
{
    int r = tss_init(task, flag, entry, esp);
    if (r < 0)
        return r;

    kernel_strcpy(task->name, name);
    task->state = TASK_CREATED;
    list_node_init(&task->task_node);
    list_node_init(&task->running_node);
    // 插入任务队列
    list_insert_front(&tss_task_queue.task_list, &task->task_node);
    // 插入就绪队列
    protect_state_t ps = protect_enter();
    tss_task_set_ready(task);
    protect_exit(ps);
    // 任务时间片初始化
    task->ticks = task->slices = TASK_DEFAULT_TICKS;
    // 延时
    task->sleep = 0;
    task->pid = task_pid++;
    task->bheap = task->eheap = 0;
    return 0;
}