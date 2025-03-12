#include <csos/memory.h>
#include <logf.h>
#include <paging.h>
#include <csos/string.h>

int num __attribute__ ((section(".mysection"))) = 1;
uint32_t gadd=0;
static memory32_info_t memory32_info;
static pde_t kernel_pde[KERNEL_PAGE_COUNT] __attribute__((aligned(PAGE_SIZE)));

static void init_memory32_info(memory32_info_t *memory32_info, uint8_t *bits, 
    uint32_t start, uint32_t size, uint32_t page_size)
{
    mutex_init(&memory32_info->mutex);
    memory32_info->size = size;
    memory32_info->page_size = page_size;
    memory32_info->start = start;
    bitmap_init(&memory32_info->bitmap, bits, size / page_size, FLASE);
}

static uint32_t memory32_alloc_page(memory32_info_t *memory32_info, uint32_t count)
{
    mutex_lock(&memory32_info->mutex);
    uint32_t addr = 0;
    int page_index = bitmap_alloc_bits(&memory32_info->bitmap, FLASE, count);
    if (page_index >= 0)
    {
        addr = memory32_info->start + page_index * memory32_info->page_size;
    }
    mutex_unlock(&memory32_info->mutex);
    return addr;
}

static void memory32_free_page(memory32_info_t *memory32_info, uint32_t addr, uint32_t count)
{
    mutex_lock(&memory32_info->mutex);
    uint32_t page_index = (addr  - memory32_info->start) / memory32_info->page_size;
    bitmap_set_bits(&memory32_info->bitmap, page_index, count, FLASE);
    mutex_unlock(&memory32_info->mutex);
}

static pte_t *find_pte(pde_t *page_dir, uint32_t vaddr, BOOL alloc)
{
    pte_t *pte;
    pde_t *pde = page_dir + PDE_INDEX(vaddr);
    if (pde->present) {
        pte = (pte_t *)PDE_ADDRESS(pde);
    } else {
        if (!alloc) {
            return NULL;
        }
        uint32_t addr = memory32_alloc_page(&memory32_info, 1);
        if (addr == 0) {
            return NULL;
        }
        pde->v = addr | PDE_P | PDE_W | PDE_U;
        pte = (pte_t *)addr;
        kernel_memset(pte, 0, PAGE_SIZE);
    }

    return pte + PTE_INDEX(vaddr);
}

// static int memory31_create_map(pde_t *pde, uint32_t vaddr, uint32_t paddr, uint32_t count, uint32_t perm)
int memory32_create_map(pde_t*pde, uint32_t vaddr, uint32_t paddr, uint32_t count, uint32_t perm)
{
   for (uint32_t i = 0; i < count; i++)
    {
        pte_t *pte = find_pte(pde, vaddr, TRUE);
        if (pte == NULL) return -1;
        pte->v = paddr | perm | PTE_P;
        vaddr += PAGE_SIZE;
        paddr += PAGE_SIZE;
    }
}

static inline uint32_t floor_page(uint32_t value)
{
    return value & ~(PAGE_SIZE - 1);
}

static inline uint32_t ceil_page(uint32_t value)
{
    return (value + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
}

static void kernel_pagging()
{
    extern uint32_t zxc_q[],qwe_q[];
    uint32_t zxc_qqawd = (uint32_t)zxc_q;
    void *zxc_qqawdw = &zxc_q;

    uint32_t qwe_qqwe = (uint32_t)qwe_q;
    void * qweqwe_q = &qwe_q;

    extern uint32_t b_text_ori[];
    extern uint32_t b_text[], e_text[], b_data[], e_data[], b_kernel[], e_kernel[],b_sysboundary[],e_sysboundary[];
    uint32_t v_text = (uint32_t)b_text_ori;
    uint32_t l_text= (uint32_t)b_text;
    uint32_t tadd = (  l_text - v_text)  ;
    uint32_t tkernel = l_text - 0x1000;
    extern uint32_t xxaasd1[],xxaasd2[],b_texta_32[];
    static memory32_map_t kernel_map[] = {
        { b_kernel, b_text, b_kernel, PTE_W },
        { b_text, e_text, b_text,PTE_U|PTE_W|PTE_U},
        { b_data, (void *)PM_EBDA_START, b_data, PTE_W },
        { (void *)PM_VGA_START, (void *)PM_VGA_STOP, (void *)PM_VGA_START, PTE_W },
        { (void *)PM_EXT_START, (void *)PM_EXT_LIMIT, (void *)PM_EXT_START, PTE_W },
        { (void *)0, (void *)0x10000, (void *)0, PTE_W }, // 栈空间要使用其中一段内存,干脆全部映射10000以下吧
        // { (void *)0x300000, (void *)0x12000, (void *)0x300000, PTE_W },
        // { (void *)0x2000, (void *)0x3000, (void *)0x400000, PTE_W },
        {  b_sysboundary,e_sysboundary,b_sysboundary,PTE_U},
        {(void*)0xFEE00300,(void*)0xFEE00301,(void*)0xFEE0030,PTE_W},
        {(void*)0xfee00900,(void*)0xfee00900,(void*)0xfee00900,PTE_W},
    };
  
    extern uint32_t  e_data_align[];
    void* bt1e_data_align = e_data_align;

    void* bt1 = b_text;
    void* bt2 = e_text;
    void* bt12 = b_data;
    void* bt22 = e_data;
    extern uint32_t _zxcd[];
    void* aqwezxcd = _zxcd;
    extern uint32_t _xxxcd[];
    void* asdqwe2 = _xxxcd;

    extern uint32_t _bbbbd;
    uint32_t asdqwe2aaaa = _bbbbd;
    void* asdqwe2aa = &_bbbbd;
    
    // extern uint32_t _abcdad;//[];
    // void* qqqqw =(    void* ) _abcdad;
    aqwezxcd++;
    void* zxc1=xxaasd1;
    void* zxc2=xxaasd2;
    void* zxc3=b_texta_32;
    uint32_t count = sizeof(kernel_map) / sizeof(memory32_map_t);
    for (int i = 0; i < count; i++)
    {
        memory32_map_t *map = &kernel_map[i];
        uint32_t vstart = floor_page((uint32_t)map->vstart);
        uint32_t vlimit = ceil_page((uint32_t)map->vlimit);
        uint32_t paddr = floor_page((uint32_t)map->pstart);
        uint32_t page_count = (vlimit - vstart) / PAGE_SIZE;

        logf("vstart: %x; vlimit: %x,paddr:%x,count:%d\n",(uint32_t)map->vstart,
        (uint32_t)map->vlimit,(uint32_t)map->pstart,page_count);
    }

    kernel_map[0].vstart = (void *) tkernel;
    kernel_map[0].pstart = (void *) tkernel;
    // (&kernel_map[0])->vstart = (void *) tkernel;
    for (int i = 0; i < count; i++)
    {
        if(i<=2||i==6){
            memory32_map_t *map = &kernel_map[i];
            map->vstart = (void*)(uint32_t)map->vstart - tadd;
            if(i<=1||i==6){
                map->vlimit = (void*)(uint32_t)map->vlimit - tadd;
            }
            map->pstart = (void*)(uint32_t)map->pstart - tadd;
        }
    }

    for (int i = 0; i < count; i++)
    {
        memory32_map_t *map = &kernel_map[i];
        uint32_t vstart = floor_page((uint32_t)map->vstart);
        uint32_t vlimit = ceil_page((uint32_t)map->vlimit);
        uint32_t paddr = floor_page((uint32_t)map->pstart);
        uint32_t page_count = (vlimit - vstart) / PAGE_SIZE;

        logf("after vstart: %x; vlimit: %x,paddr:%x,count:%d\n",(uint32_t)map->vstart,
        (uint32_t)map->vlimit,(uint32_t)map->pstart,page_count);
        logf("xxxx after vstart: %x; vlimit: %x,paddr:%x,count:%d\n",vstart,
        vlimit,paddr,page_count);
    }

    for (int i = 0; i < count; i++)
    {
        memory32_map_t *map = &kernel_map[i];
        uint32_t vstart = floor_page((uint32_t)map->vstart);
        uint32_t vlimit = ceil_page((uint32_t)map->vlimit);
        uint32_t paddr = floor_page((uint32_t)map->pstart);
        uint32_t page_count = (vlimit - vstart) / PAGE_SIZE;

        memory32_create_map(kernel_pde, vstart, paddr, page_count, map->perm);
    }
}

int copy_page(uint32_t index)
{
    uint32_t to_pde = memory32_create_pde();
    if (to_pde == 0) {
        return -1;
    }

    uint32_t pde_start = PDE_INDEX(VM_TASK_BASE);
    pde_t *pde = (pde_t *)index + pde_start;
    for (int i = pde_start; i < PDE_COUNT; i++, pde++)
    {
        if (!pde->present) {
            continue;
        }
        pte_t *pte = (pte_t *)PDE_ADDRESS(pde);
        for (int k = 0; k < PTE_COUNT; k++, pte++)
        {
            if (!pte->present) {
                continue;
            }
            uint32_t page = memory32_alloc_page(&memory32_info, 1);
            if (page == 0) {
                destroy_page(to_pde);
                return -1;
            }

            uint32_t vaddr = (i << 22) | (k << 12);
            int ret = memory32_create_map((pde_t *)to_pde, vaddr, page, 1, PTE_PERM(pte));
            if (ret < 0) {
                destroy_page(to_pde);
                return -1;
            }
            kernel_memcpy((void *)page, (void *)vaddr, PAGE_SIZE);
        }
    }
    return to_pde;
}

void destroy_page(uint32_t index)
{
    uint32_t pde_start = PDE_INDEX(VM_TASK_BASE);
    pde_t *pde = (pde_t *)index + pde_start;
    for (int i = pde_start; i < PDE_COUNT; i++, pde++) {
        if (!pde->present) {
            continue;
        }
        pte_t *pte = (pte_t *)PTE_ADDRESS(pde);
        for (int k = 0; k < PTE_COUNT; k++, pte++) {
            if (!pte->present) {
                continue;
            }
            memory32_free_page(&memory32_info, (uint32_t)PTE_ADDRESS(pte), 1);
        }
    }
    memory32_free_page(&memory32_info, (uint32_t)PDE_ADDRESS(pde), 1);
}

uint32_t memory32_create_pde()
{
    pde_t *pde = (pde_t *)memory32_alloc_page(&memory32_info, 1);
    if (pde == 0) return 0;
    kernel_memset((void *)pde, 0, PAGE_SIZE);
    const uint32_t user_pde_start = PDE_INDEX(VM_TASK_BASE);
    // 映射内核页表
    for (int i = 0; i < user_pde_start; i++)
    {
        pde[i].v = kernel_pde[i].v;
    }
    return (uint32_t)pde;
}

uint32_t memory32_get_paddr(uint32_t pde, uint32_t vaddr)
{
    pte_t *pte = find_pte((pde_t *)pde, vaddr, FLASE);
    if (!pte) return 0;
    return PTE_ADDRESS(pte) + (vaddr & (PAGE_SIZE - 1));
}

int memory32_copy_page_data(uint32_t to, uint32_t pde, uint32_t from, uint32_t size)
{
    while (size > 0)
    {
        uint32_t to_paddr = memory32_get_paddr(pde, to);
        if (to_paddr == 0) {
            return -1;
        }
        uint32_t offset_in_page = to_paddr & (PAGE_SIZE - 1);
        uint32_t current_size = PAGE_SIZE - offset_in_page;
        if (current_size > size) {
            current_size = size;
        }
        kernel_memcpy((void *)to_paddr, (void *)from, current_size);
        size -= current_size;
        to += current_size;
        from += current_size;
    }
    return 0;
}

static int memory32_alloc_task_pages(uint32_t pde, uint32_t index, uint32_t size, uint32_t perm)
{
    uint32_t current_index = index;
    int count = ceil_page(size) / PAGE_SIZE;

    for (int i = 0; i < count; i++)
    {
        uint32_t paddr = memory32_alloc_page(&memory32_info, 1);   
        if (paddr == 0) {
            // tty_printf("memory allocate failed.");
            return -1;
        }

        int error = memory32_create_map((pde_t *)pde, current_index, paddr, 1, perm);
        if (error < 0) {
            // tty_printf("memory create failed.");
            return -1;
        }

        current_index += PAGE_SIZE;
    }

    return 0;
}

uint32_t alloc_page()
{
    return memory32_alloc_page(&memory32_info, 1);
}

void free_page(uint32_t addr)
{
    extern uint8_t b_init_task[];
    if (addr < (uint32_t)b_init_task) {
        memory32_free_page(&memory32_info, addr, 1);
    } else {
        pte_t *pte = NULL;
        #ifdef TASK_TSS
            pte = find_pte((pde_t *)get_running_task()->tss.cr3, addr, FLASE);
        #endif

        #ifdef TASK_SIMPLE
            pte = find_pte((pde_t *)get_running_task()->pde, addr, FLASE);
        #endif
        memory32_free_page(&memory32_info, PTE_INDEX((uint32_t)pte), 1);
        pte->v = 0;
    }
}

int alloc_pages(uint32_t pde, uint32_t index, uint32_t size, uint32_t perm)
{
    return memory32_alloc_task_pages(pde, index, size, perm);
}

void memory_init(memory_info_t *memory_info)
{
    uint32_t total_size = 0;
    for (int i = 0; i < memory_info->count; i++) {
        memory_raw_t *raw = &memory_info->raws[i];
        logf("p.address: %#x, size: %#x, type = %d",
            raw->base_l, raw->length_l, raw->type);
        if (raw->type == 1)
            total_size += raw->length_l;
    }
    total_size -= PM_EXT_START;
    total_size = floor_page(total_size);
    logf("total available memory size: %#x", total_size);
    // 用户内存开始位置
    extern uint32_t memory_start[];
    uint32_t total_start = (uint32_t)&memory_start; // loadaddr

    extern uint32_t b_text_ori[],b_text[];  // vaddr,loadaddr
    uint32_t v_text = (uint32_t)b_text_ori;
    uint32_t l_text = (uint32_t)b_text;
    uint32_t tadd = (l_text - v_text);
    gadd = tadd;
    total_start = total_start - tadd; // vaddr

    // 每页占用1bit, 计算位图大小
    total_start += bitmap_bit_size(total_size / PAGE_SIZE);
    total_start = ceil_page(total_start);// 优化下,位图页对齐
    // 感觉之前的代码这里有问题,需要加上total_start += bitmap_bit_size吗,我看了位图的算法又不是按倒叙来计算的啊!???
    // 严谨点的话这里需要判断total_start+bitmap_bit_size(total_size / PAGE_SIZE)是否超过 PM_EBDA_START
    // 因为我看内核映射数据段那里limit是PM_EBDA_START
    if(total_start + bitmap_bit_size(total_size / PAGE_SIZE) > PM_EBDA_START){
        logf("error: %d\n", __LINE__);
        while (1){};
    }

    // 忽略偏移量，计算最大内存页的地址
    total_size &= 0xFFFFF000;
    // 初始化内存位图
    logf("memory32_info: %#x\t%#x\n", memory32_info.size,memory32_info.page_size);
    init_memory32_info(&memory32_info, 
        (uint8_t *)total_start, 
        PM_EXT_START, 
        total_size, 
        PAGE_SIZE);
    logf("memory32_info: %#x\t%#x\n", memory32_info.size,memory32_info.page_size);
    for (int i = 0; i < 5; i++)
    {
        logf("xx memory32_%d:%#x\n",  i,memory32_info.bitmap.bits[i]);
    }
    
    logf("total available memory start: %#x", total_start);
    //    logf("total available memory start: %#x", PM_EXT_START); 难道不是这行才是正确的吗
    // 映射内核页
    kernel_pagging();
    logf("yy memory32_info: %#x\t%#x\n", memory32_info.size,memory32_info.page_size);
    for (int i = 0; i < 5; i++)
    {
        logf("yy memory32_%d:%#x\n",  i,memory32_info.bitmap.bits[i]);
    }

    // 设置内核页
    set_pde((uint32_t)kernel_pde);
    write_cr0(read_cr0() | (1 << 31));
    logf("zz memory32_info: %#x\t%#x\n", memory32_info.size,memory32_info.page_size);
    for (int i = 0; i < 5; i++)
    {
        logf("zz memory32_%d:%#x\n",  i,memory32_info.bitmap.bits[i]);
    }

}