#include <stdarg.h>
#include <syscall.h>

int vsprintf(char *buf, const char *fmt, va_list args);

int a=1;
//typedef unsigned int uint32_t;

int testa(){
    a++;
}
int b=1;
unsigned short int line = 0x1e0;
void show_string2(char *str)
{
    unsigned short int tmpline = line;
    for (char *p = str; *p != 0; p++) {
        char c = (*p & 0xFF);
        if(c=='\r'){
            continue;
        }

        if(c=='\n'){
            tmpline = 0xa0*((tmpline / 0xa0)+1);
            continue;
        }

        __asm__ __volatile__(
            "push %%gs\n"
            "mov %%bx,%%di\n"
            "mov $0x40,%%ax\n"
            "mov %%ax,%%gs\n"
            "mov $0x26, %%ah\n"
            "mov %[c], %%al\n"
            "mov %%ax, %%gs:(%%di)\n\t"
            "pop %%gs\n"
            :: [c]"r"(c),"b"(tmpline): "%di","%ah", "%al");
        tmpline+=2;
    }

    if((tmpline+0)%0xa0!=0){
        line = 0xa0*((tmpline / 0xa0)+1);
    }else{
        line = tmpline;
    }
   
    // __asm__ __volatile__("xchg %bx,%bx\n");
    if(line>0xa0*30){
        line = 0x0;
        b++;
    }
}


int c2=0;
void funcaa(){
    int sign32 = 0x534D4150, sign16 = 0xE820;
    int index = 0, signature, bytes;
    int d=2;
    int*pc=&c2;
    d++;
    //    "mov %eax,%%ebx"
    __asm__ __volatile__("mov $1,%%eax\n"
    "mov $1,%%eax\n"
      "mov $1,%%eax\n\t"
            "mov $1,%%eax\n\t"
            "movb $1,1(%%eax)"
    : "=a"(signature)
    : "a"(sign16));

}

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long long int uint64_t;
typedef unsigned int size_t;
#pragma pack(1)
typedef struct  {
    unsigned short limit;       // GDT的限制
    unsigned int  base;      // GDT的基地址
}GDTR;

void getGDTR(GDTR* gdtr) {
    __asm__ __volatile__ (
        "sgdt %0" : "=m" (*gdtr) : : "memory"
    );
}

void printHex3(uint32_t h){
    char str1[50]={"gdtaddr:"};
    size_t j = 8;
    char* s= (char*) (&h);
    for (size_t i = 0; i < sizeof(h); i++)
    {
        unsigned char ao=*s++;
        unsigned char ao1=ao/16;
        unsigned char ao2=ao%16;
        if(ao1<10){
            str1[j++] = ao1 + '0';
        }else{
            str1[j++]= (ao1 -10) + 'a';
        }

        if(ao2<10){
            str1[j++] = ao2 + '0';
        }else{
            str1[j++]= (ao2 -10) + 'a';
        }
        //  __asm__ __volatile__("xchg %bx,%bx\n");
    }
    str1[j++] =' ';
    str1[j++] =' ';

    s= (char*) (&h)+sizeof(h)-1;
    for (size_t i = 0; i < sizeof(h); i++)
    //for (size_t i = sizeof(h)-1; i>=0; i--)
    // for (size_t i = sizeof(h); i>0; i--)
    {
        unsigned char ao=*s--;
        unsigned char ao1=ao/16;
        unsigned char ao2=ao%16;
        if(ao1<10){
            str1[j++] = ao1 + '0';
        }else{
            str1[j++]= (ao1 -10) + 'a';
        }

        if(ao2<10){
            str1[j++] = ao2 + '0';
        }else{
            str1[j++]= (ao2 -10) + 'a';
        }
        //  __asm__ __volatile__("xchg %bx,%bx\n");
    }
    str1[j++] = 0;
    show_string2(str1);
}

extern void pecpy(uint16_t seg_src,uint32_t addr,uint32_t len,uint16_t seg_dst,uint32_t addr2,uint32_t len2);

void read_disk(uint32_t sector, uint32_t count, uint16_t* buffer)
{
    outb(0x1F6, 0xE0);
    outb(0x1F2, (uint8_t)(count >> 8));
    outb(0x1F3, (uint8_t)(count >> 24));
    outb(0x1F4, 0);
    outb(0x1F5, 0);

    outb(0x1F2, (uint8_t)count);
    outb(0x1F3, (uint8_t)sector);
    outb(0x1F4, (uint8_t)(sector >> 8));
    outb(0x1F5, (uint8_t)(sector >> 16));

    outb(0x1F7, 0x24);

    uint16_t *ptr = buffer;
    while(count --) {
        while((inb(0x1F7) & 0x88) != 0x8) ;
        for (int i = 0; i < 512 / 2; i++) {
            *ptr++ = inw(0x1F0);
        }
    }
}

/*==================Memory Check==================*/

#define MEMORY_MAX_COUNT    24
#pragma pack(1)
typedef struct memory_raw_t2
{
    uint32_t base_l;
    uint32_t base_h;
    uint32_t length_l;
    uint32_t length_h;
    uint32_t type;
} memory_raw_t2;

#pragma pack(1)
typedef struct memory_info_t2
{
    memory_raw_t2 raws[MEMORY_MAX_COUNT];
    uint32_t count;
} memory_info_t2;

// 当前代码不在这里使用,只是通过gcc编译一下,然后拿到汇编代码,在boot2.asm中使用
void memory_check()
{
    memory_info_t2 memory_info;
    int sign32 = 0x534D4150, sign16 = 0xE820;
    int index = 0, signature, bytes;
    memory_info.count = 0;
    while (1) {
        memory_raw_t2 *memory_raw = &memory_info.raws[index];
        __asm__ __volatile__("int $0x15" 
				: "=a"(signature), "=c"(bytes), "=b"(index)
				: "a"(sign16), "b"(index), "c"(MEMORY_MAX_COUNT), "d"(sign32), "D"(memory_raw));
        if (signature != sign32) {
            //show_string2("Memory check error!\r\n");
        }
        memory_info.count ++;
        if (index == 0) {
            //show_string2("Memory check success!\r\n");
            break;
        }
    }

    // char *s =( char *)&memory_info;
    // for (size_t i = 0; i < sizeof(memory_info_t); i++)
    // {
    //    *dst++ = *s++;
    // }
    
    __asm__ __volatile__("xchg %bx,%bx\n");
}

memory_info_t2 gmemory_info_t;

static inline int sprintf3(char buffer[],const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buffer, fmt, args);
    va_end(args);
    // syscall_arg_t printf_arg = { SYS_NR_PRINTF, (uint32_t)buffer, i, 0, 0 };
    // __asm__ __volatile__("xchg %bx,%bx\nnop\nnop\nnop\n");
    return 0;//_syscall(&printf_arg);
}

void pememcpy(){
    __asm__ __volatile__("xchg %bx,%bx\nnop\nnop\nnop\nnop\n");

    __asm__ __volatile__(
    "pusha\n"
    "movw $0x50,%ax\n"
    "movw %ax,%gs\n"
    "movl $0x40000000,%eax\n"
    "movl %gs:(%eax),%ebx\n"
    "movl $0x123,%edx\n"
    "movl %edx, %gs:(%eax)\n"
    "movl %gs:(%eax),%ecx\n"
    "movl $0x300000,%esi\n"
    "movl $0x123, %gs:(%esi)\n"
    "movl $0x300004,%esi\n"
    "movl %ecx, %gs:(%esi)\n"
    "popa\n"
    );

    // char str2[]={"Memory check start!\r\n\0"};
    show_string2("Memory check start!");

    char *src =( char *)0x200;
    char *dst =( char *)&gmemory_info_t;
    for (size_t i = 0; i < sizeof(memory_info_t2); i++)
    {
       *dst++ = *src++;
    }

    char caa[1024];
    sprintf3(caa,"form\nat:%d\n",3);
    show_string2(caa);

    for (size_t i = 0; i < gmemory_info_t.count; i++)
    {
        memory_raw_t2 r = gmemory_info_t.raws[i];
        sprintf3(caa,"index:%d/%d,addr:%X,limit:%X,type:%d\n",i+1,gmemory_info_t.count,
            r.base_l,r.length_l,r.type);
        show_string2(caa);
        // __asm__ __volatile__("xchg %bx,%bx\n");
    }
    
    __asm__ __volatile__("xchg %bx,%bx\n");
    //memory_check();
    read_disk(35,400,(uint16_t*)0x7000);

    read_disk(500,400,(uint16_t*)0x200000);
    
    read_disk(1000+(0x400/0x200),10,(uint16_t*)0x8000);

    __asm__ __volatile__("xchg %bx,%bx\n");
    GDTR gdtr;
    size_t a=sizeof(gdtr);
    getGDTR(&gdtr);
    sprintf3(caa,"gdtaddr:%X,limit:%X",gdtr.base,gdtr.limit);
    show_string2(caa);
    show_string2("aaaaaaaaaaaaaaa");

    __asm__ __volatile__("xchg %bx,%bx\n");
    pecpy(0x20,0x7000,0x4c00,0x20,0x00400000,0xc65000);

    return;
}

