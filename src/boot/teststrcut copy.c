#include <stdio.h>
//#include <windows.h>

#define printf _printf

void _printf(const char * fmt,...){

}

#define GDT_SIZE            0x100

#define SEG_ATTR_G          (1 << 15)
#define SEG_ATTR_D          (1 << 14)
#define SEG_ATTR_P          (1 << 7)

#define SEG_ATTR_DPL0       (0 << 5)
#define SEG_ATTR_DPL3       (3 << 5)

#define SEG_ATTR_CPL0       (0 << 0)
#define SEG_ATTR_CPL3       (3 << 0)

#define SEG_SYSTEM          (0 << 4)
#define SEG_NORMAL          (1 << 4)
#define SEG_TYPE_DATA       (0 << 3)
#define SEG_TYPE_CODE       (1 << 3)
#define SEG_TYPE_RW         (1 << 1)
#define SEG_TYPE_TSS        (9 << 0)

#define GATE_TYPE_IDT		(0xE << 8)		// 中断32位门描述符
#define GATE_TYPE_SYSCALL   (0xC << 8)		// 系统调用32位门描述符
#define GATE_ATTR_P 		(1 << 15)		// 是否存在
#define GATE_ATTR_DPL0		(0 << 13)		// 特权级0，最高特权级
#define GATE_ATTR_DPL3		(3 << 13)		// 特权级3，最低权限

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#pragma pack(1)
typedef struct gdt_table_t
{
    uint16_t limit_l;
    uint16_t base_l;
    uint8_t base_m;
    uint16_t attr;
    uint8_t base_h;
} gdt_table_t;

gdt_table_t gdt_table[123] = {
    {0, 0, 0, 0}
};

void set_gdt_table_entry(int selector, uint32_t base, uint32_t limit, uint16_t attr)
{
    if (limit > 0xFFFFF) {
        attr |= 0x8000;
        limit >>= 12;
        printf("g123\n");
    }
    gdt_table_t *entry = &gdt_table[selector >> 3];
    entry->limit_l = limit & 0xFFFF;
    entry->base_l = base & 0xFFFF;
    entry->base_m = (base >> 16) & 0xFF;
    entry->attr = attr | (((limit >> 16) & 0xF) << 8);
    entry->base_h = (base >> 24) & 0xFF;
}

void memcpy2(uint8_t*dst,uint8_t*src,size_t len){
    for(int i=0;i<len;i++){
        *dst++ =  *src++;
    }
}

#define LOOPLINE while(1){  static int _i = 1; if(_i){_i=0;printf("LOOP _LINE_:%d\n",__LINE__);} }

#define UINT32_MAX 0xFFFFFFFF
uint8_t will_overflow(uint32_t a, uint32_t b) {
    return (a > UINT32_MAX - b);
}
void pecpy(uint8_t seg_src,uint32_t addr,uint32_t len){
    uint8_t seg_ds = 1*8;
    seg_src = 2*8;
    //???
    uint8_t* gdt_addr_base2 = (uint8_t*)gdt_table;
    uint32_t gdt_addr_base=0;
    //   __asm__ __volatile__(
    //    "mov %%ax,%%ds\n"
    //    :"=a"(seg_ds));

    //uint8_t* target_seg_addr = gdt_addr_base+4*(seg_ds&0xFFFF>>3);
    uint32_t target_seg_addr =0;// gdt_addr_base2+4*(seg_ds&0xFFFF>>3);
    uint32_t src_seg_addr = gdt_addr_base+4*(seg_src&0xFFFF>>3);
    uint32_t gdt_table_item_h,gdt_table_item_l;
    uint32_t src_gdt_table_item_h  ,src_gdt_table_item_l;

    int cpp=0;
    printf("cpp:%d\n",cpp++);
    //src_gdt_table_item_h =  (uint32_t)&gdt_table[1];
    //src_gdt_table_item_l = &gdt_table[i]

    //while(1){}
    //return ;

    //  __asm__ __volatile__(
    //    "mov $0,%%cx\n"
    //    "mov %%cx, %%gs\n"
    //    "movl (%%eax),%%ebx\n"
    //    "movl 4(%%eax),%%ecx\n"
    //    "movl (%%edx),%%esi\n"
    //    "movl 4(%%edx),%%edi\n"
    //    :"=b"(gdt_table_item_l),"=c"(gdt_table_item_h),"=S"(src_gdt_table_item_l),"=D"(src_gdt_table_item_h)
    //    :"a"(target_seg_addr),"d"(src_seg_addr));

    gdt_table_t gdt=gdt_table[1];
    // method 1
    gdt.limit_l = src_gdt_table_item_l|0xFFFF;
    gdt.base_l = src_gdt_table_item_l >> 16 |0xFFFF;
    // method 2
    uint32_t tmparr[2];
    tmparr[0] = src_gdt_table_item_l;
    tmparr[1] = src_gdt_table_item_h;
    gdt_table_t *pgdt =(gdt_table_t *)tmparr ; 
    pgdt = &gdt_table[1];
    uint32_t tmparr2[2];
    tmparr2[0] = gdt_table_item_l;
    tmparr2[1] = gdt_table_item_h;
    gdt_table_t *ptarget_gdt =(gdt_table_t *)tmparr2 ; 
    ptarget_gdt = &gdt_table[2];
    printf("cpp:%d\n",__LINE__ );
    uint32_t src_baseadd = pgdt->base_l&0xFFFF;
    src_baseadd = src_baseadd | (( pgdt->base_m&0xFF&0xFFFFFFFF) <<16 & 0xFFFFFFFF);
    src_baseadd = src_baseadd | (( pgdt->base_h&0xFF&0xFFFFFFFF) <<24 & 0xFFFFFFFF);
    uint32_t target_baseadd = ptarget_gdt->base_l&0xFFFF;
    target_baseadd = target_baseadd | (( ptarget_gdt->base_m&0xFF&0xFFFFFFFF) <<16 & 0xFFFFFFFF);
    target_baseadd = target_baseadd | (( ptarget_gdt->base_h&0xFF&0xFFFFFFFF) <<24 & 0xFFFFFFFF);
printf("cpp:%d\n",__LINE__ );
    uint32_t  src_limit =( pgdt->attr & 0xFFFF)>>8 & 0x0F;
      printf("src_limit:%x\n",src_limit);
printf("src_limit:%x\n",   ((src_limit &0x0FFFFF )<< 16));
   
    src_limit =    ((src_limit &0x0FFFFF) << 16) | (pgdt->limit_l&0xFFFF);
       // src_limit =    (pgdt->limit_l&0xFFFF);
            printf("src_limit:%x\n",src_limit);
    uint8_t g3 = (pgdt->attr & (1<<15)) >> 15;
     uint16_t g = pgdt->attr & (1<<15);//) >> 15;
     printf("attr=%x\tg=%x\tg3=%x\n",pgdt->attr,g,g3);
    if(g!=0){
        src_limit = src_limit << 12;
        printf("aaaaaaaaaaaaaa\n");
    }
printf("cpp:%d\n",__LINE__ );
    uint32_t  target_limit =( ptarget_gdt->attr & 0xFFFF)>>8 & 0x0F;
    target_limit =    ((target_limit &0x0FFFFF) << 16) | (ptarget_gdt->limit_l&0xFFFF);
    uint8_t g2 = (ptarget_gdt->attr & (1<<15)) >> 15;
    if(g2){
        target_limit = target_limit * 0x1000;
    }
    
    printf("cpp:%d\n",__LINE__ );
   if(will_overflow(src_baseadd,addr)){
        while(1){};
   }
    printf("cpp:%d\n",__LINE__ );
   if(will_overflow(src_baseadd+addr,len)){
           printf("xxx:%x+%x\t%x\n",src_baseadd,addr, len);
        printf("xxx:%x+%x\n",src_baseadd+addr, len);
        while(1){};
   }
printf("cpp:%d\n",__LINE__ );
   // 判断长度是否足够
   uint32_t src_max_add;
   if(will_overflow(src_baseadd,src_limit)){
       src_max_add = 0xFFFFFFFF;
   }else{
       printf("qwqwe:%x+%x+%x\n",src_max_add,src_baseadd,src_limit);
       src_max_add = src_baseadd+src_limit;
   }
printf("cpp:%d\n",__LINE__ );
   if(src_baseadd+addr+len>src_max_add){
        printf("xxx:%x+%x+%x\t%x\n",src_baseadd,addr,len, src_max_add);
LOOPLINE
   }
printf("cpp:%d\n",__LINE__ );
   uint32_t  src_py_addr = src_baseadd+addr;
   if(src_py_addr < target_baseadd){
    printf("src_py_addr:%x,target_baseadd:%x\n",src_py_addr,target_baseadd );
LOOPLINE
   }
printf("cpp:%d\n",__LINE__ );
   uint32_t reloc_addr = src_py_addr - target_baseadd;
   // 长度不足2
   if(reloc_addr>target_limit){
       printf("qwe:%x\t%x\n",reloc_addr ,target_limit);
       LOOPLINE
   }
printf("cpp:%d\n",__LINE__ );
   uint32_t target_max_add;
   if(will_overflow(target_baseadd,target_limit)){
       target_max_add = 0xFFFFFFFF;
   }else{
       target_max_add = target_baseadd+target_limit;
   }
   printf("cpp:%d\n",__LINE__ );
   // 长度不足3
   if(target_baseadd+reloc_addr>target_max_add){
           while(1){};
   }
    printf("cpp:%d\n",__LINE__ );

printf("qwe:%x+%x=%x\n",target_baseadd,reloc_addr,src_py_addr);
printf("qwe:%x+%x=%x\n",src_baseadd,addr,src_py_addr);

    //mov ebx,1
    //mov ds,1
    typedef struct {
        uint32_t raw_offset;
        uint32_t dst_img;
        uint32_t len;
    } _t;

    _t exe_arr[]={{1,2,3}};
    for(int i=0;i<sizeof(exe_arr)/sizeof(exe_arr[0]);i++){
        memcpy2((uint8_t*)((void*)exe_arr[i].dst_img),(uint8_t*)((void*)exe_arr[i].raw_offset),exe_arr[i].len);
    }
printf("cpp:%d\n",__LINE__ );
}


//typedef unsigned int uint32_t;
int pememcpy(){
    set_gdt_table_entry(1*8, 0x123400, 0x6F100000, 
        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_NORMAL | SEG_TYPE_DATA  | SEG_TYPE_RW | SEG_ATTR_D | SEG_ATTR_G);
    set_gdt_table_entry(2*8, 0x56780, 0x5F400000, 
        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_NORMAL | SEG_TYPE_DATA  | SEG_ATTR_D );
    printf("xxxx:%d\n",sizeof(gdt_table[0]));

    for(int i=0;i<sizeof(gdt_table[0]);i++){
        uint8_t * t= ((uint8_t*)(&gdt_table[1]))+i;
        uint8_t t2 = *t;
        for(int j=0;j<8;j++){
          printf("%d ",(t2>>j)&1);
        }
        printf("\n");
    }
    printf("yyyy\n");

    
    for(int i=0;i<sizeof(gdt_table[0]);i++){
        uint8_t * t= ((uint8_t*)(&gdt_table[2]))+i;
        uint8_t t2 = *t;
        for(int j=0;j<8;j++){
          printf("%d ",(t2>>j)&1);
        }
        printf("\n");
    }
    printf("zzzz\n");
    pecpy(1*8,0x123456,0x10000);
    while(1){};

    uint32_t  src_limit =0xff12 & 0xFFFF>>8 & 0x0F;
     //src_limit=0x123456;
    src_limit =    ((src_limit &0x0FFFFF) << 16) | (0x1234&0xFFFF);
               printf("src_limit:%x\n",  src_limit);

src_limit =0xff12 & 0xFFFF>>8 & 0x0F;
//src_limit=0x123456;
    printf("src_limit:%x\n",  src_limit );
    src_limit =    ((src_limit &0x0FFFFF) << 16) | (0x1234&0xFFFF);
               printf("src_limit:%x\n",  src_limit);

    return 0;
}