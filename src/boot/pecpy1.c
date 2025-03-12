
// #include <stdio.h>
// int main(){
//     return 0;
// }

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long long int uint64_t;
typedef unsigned int size_t;
void memcpy2(uint8_t*dst,uint8_t*src,size_t len){
    for(int i=0;i<len;i++){
        *dst++ =  *src++;
    }
}

#define LOOPLINE while(1){  static int _i = 1; if(_i){_i=0;printf2("LOOP _LINE_:%d\n",__LINE__);} }
void show_string2(char *str);
void printf2(const char *c,uint32_t line){
    char str1[50]={"LOOP _LINE_:"};
    size_t j = 12;
    uint32_t num = line;
    // num=123;
    while (num > 0) {
        uint32_t digit = num % 10;
        str1[j++] = (char)('0' + digit);//ao1 + '0';
        // result = char('0' + digit) + result;
        num /= 10;
    }

    str1[j++] =0;
    show_string2(str1);
}

#define UINT32_MAX 0xFFFFFFFF
uint8_t will_overflow(uint32_t a, uint32_t b) {
    return (a > UINT32_MAX - b);
}

#pragma pack(1)
typedef struct gdt_table_t
{
    uint16_t limit_l;
    uint16_t base_l;
    uint8_t base_m;
    uint16_t attr;
    uint8_t base_h;
} gdt_table_t;

#pragma pack(1)
typedef struct  {
    unsigned short limit;       // GDT的限制
    unsigned int  base;      // GDT的基地址
}GDTR2;

void getGDTR2(GDTR2* gdtr) {
    __asm__ __volatile__ (
        "sgdt %0" : "=m" (*gdtr) : : "memory"
    );
}

void printHex3(uint32_t h);
uint32_t calca_addr(uint16_t seg_src,uint32_t addr,uint32_t len);
void pecpy(uint16_t seg_src,uint32_t addr,uint32_t len,uint16_t seg_dst,uint32_t addr2,uint32_t len2){
    uint32_t reloc_addr_src = calca_addr(seg_src, addr, len);
    uint32_t reloc_addr_dst = calca_addr(seg_dst, addr2, len2);
    //mov ebx,1
    //mov ds,1
    typedef struct {
        uint32_t raw_offset;
        uint32_t dst_img;
        size_t len;
    } _t;

    _t exe_arr[]={
        { 0x00000400,  0x00411000-0x400000,0x0001b6f2},
        { 0x0001bc00,  0x0042d000-0x400000,0x00004a3a},
        { 0x00020800,  0x00432000-0x400000,0x00000800},
        { 0x00021000,  0x01051000-0x400000,0x00002032},
        { 0x00023200,  0x01054000-0x400000,0x000002ea},
        { 0x00023600,  0x01055000-0x400000,0x0000010e},
        { 0x00023800,  0x01056000-0x400000,0x0000043c},
        { 0x00023e00,  0x01057000-0x400000,0x000073a3},
    };

    for (size_t i = 0; i < sizeof(exe_arr)/sizeof(exe_arr[0]); i++)
    {
        exe_arr[i].raw_offset +=reloc_addr_src;
        exe_arr[i].dst_img +=reloc_addr_dst;
    }

    for(int i=0;i<sizeof(exe_arr)/sizeof(exe_arr[0]);i++){
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
        memcpy2((uint8_t*)((void*)(exe_arr[i].dst_img)),(uint8_t*)(exe_arr[i].raw_offset),exe_arr[i].len);
        #pragma GCC diagnostic pop
    }

    int dd=2;
    dd++;
    dd++;
}

uint32_t calca_addr(uint16_t seg_src,uint32_t addr,uint32_t len){
    //__asm__ __volatile__("xchg %bx,%bx\n");
    uint16_t seg_ds;
    // __asm__ __volatile__(
    //     "mov %%ds,%%ax\n"
    //     :(seg_ds)"=a"
    // );
    __asm__("mov %%ds, %0" : "=r" (seg_ds));
    //???
    uint32_t gdt_addr_base;
    GDTR2 gdtr;
    size_t a=sizeof(gdtr);
    getGDTR2(&gdtr);
    gdt_addr_base = gdtr.base;
    printHex3(gdt_addr_base);
    uint32_t target_seg_addr = gdt_addr_base+8*((seg_ds&0xFFFF)>>3);
    uint32_t src_seg_addr = gdt_addr_base+8*((seg_src&0xFFFF)>>3);
    uint32_t gdt_table_item_h,gdt_table_item_l;
    uint32_t src_gdt_table_item_h,src_gdt_table_item_l;
    printHex3(target_seg_addr);
    __asm__ __volatile__("xchg %bx,%bx\n");
    __asm__ __volatile__(
        "mov $0,%%cx\n"
        "mov %%cx, %%gs\n"
        "xchg %%bx,%%bx\n"
        "movl (%%eax),%%ebx\n"
        "movl 4(%%eax),%%ecx\n"
        "movl (%%edx),%%esi\n"
        "movl 4(%%edx),%%edi\n"
        :"=b"(gdt_table_item_l),"=c"(gdt_table_item_h),"=S"(src_gdt_table_item_l),"=D"(src_gdt_table_item_h)
        :"a"(target_seg_addr),"d"(src_seg_addr));

    gdt_table_t gdt;
    // method 1
    gdt.limit_l = src_gdt_table_item_l|0xFFFF;
    gdt.base_l = src_gdt_table_item_l >> 16 |0xFFFF;
    // method 2
    uint32_t tmparr[2];
    tmparr[0] = src_gdt_table_item_l;
    tmparr[1] = src_gdt_table_item_h;
    gdt_table_t *pgdt =(gdt_table_t *)tmparr ; 
    uint32_t tmparr2[2];
    tmparr2[0] = gdt_table_item_l;
    tmparr2[1] = gdt_table_item_h;
    gdt_table_t *ptarget_gdt =(gdt_table_t *)tmparr2 ; 

    uint32_t src_baseadd = pgdt->base_l&0xFFFF;
    src_baseadd = src_baseadd | (( pgdt->base_m&0xFF&0xFFFFFFFF) <<16 & 0xFFFFFFFF);
    src_baseadd = src_baseadd | (( pgdt->base_h&0xFF&0xFFFFFFFF) <<24 & 0xFFFFFFFF);

    printHex3(src_baseadd);

    uint32_t target_baseadd = ptarget_gdt->base_l&0xFFFF;
    target_baseadd = target_baseadd | (( ptarget_gdt->base_m&0xFF&0xFFFFFFFF) <<16 & 0xFFFFFFFF);
    target_baseadd = target_baseadd | (( ptarget_gdt->base_h&0xFF&0xFFFFFFFF) <<24 & 0xFFFFFFFF);
    printHex3(target_baseadd);
    uint32_t  src_limit = (pgdt->attr & 0xFFFF)>>8 & 0x0F;
    src_limit =    ((src_limit &0x0FFFFF) << 16) | (pgdt->limit_l&0xFFFF);
    uint16_t g = pgdt->attr & (1<<15);
    if(g){
        src_limit = src_limit * 0x1000;
    }

    uint32_t  target_limit = (ptarget_gdt->attr & 0xFFFF)>>8 & 0x0F;
    target_limit =    ((target_limit &0x0FFFFF) << 16) | (ptarget_gdt->limit_l&0xFFFF);
    uint16_t g2 = ptarget_gdt->attr & (1<<15);
    if(g2){
        target_limit = target_limit * 0x1000;
    }

   if(will_overflow(src_baseadd,addr)){
    LOOPLINE
   }

   if(will_overflow(src_baseadd+addr,len)){
    LOOPLINE
   }

   // 判断长度是否足够
   uint32_t src_max_add;
   if(will_overflow(src_baseadd,src_limit)){
       src_max_add = 0xFFFFFFFF;
   }else{
       src_max_add = src_baseadd+src_limit;
   }
   if(src_baseadd+addr+len>src_max_add){
       LOOPLINE // 长度不足1
   }

   uint32_t  src_py_addr = src_baseadd+addr;
   printHex3(src_baseadd);
   printHex3(addr);
   printHex3(src_py_addr);
   printHex3(target_baseadd);
   if(src_py_addr < target_baseadd){
       LOOPLINE
   }

   uint32_t reloc_addr = src_py_addr - target_baseadd;
   // 长度不足2
   if(reloc_addr>target_limit){
      LOOPLINE
   }
   uint32_t target_max_add;
   if(will_overflow(target_baseadd,target_limit)){
       target_max_add = 0xFFFFFFFF;
   }else{
       target_max_add = target_baseadd+target_limit;
   }
   
   // 长度不足3
   if(target_baseadd+reloc_addr>target_max_add){
          LOOPLINE
   }

    //    printHex3(target_baseadd);
    show_string2("xxxxxxxxxxx");
    printHex3(reloc_addr);
    return reloc_addr;
}
