#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

typedef struct
{
	bool e;
	bool w;
}DataSegInfo;
 
typedef struct
{
	bool c, r;
}CodeSegInfo;
 
typedef struct
{
	bool is;
	bool a;
	DataSegInfo di;
	CodeSegInfo ci;
}TYPEData;
 
typedef struct
{
	UINT32 limit;
	UINT32 base;
	UINT32 TYPE;
	TYPEData typedata;
	UINT32 S;
	UINT32 DPL;
	UINT32 P;
	UINT32 AVL;
	UINT32 L;			//保留位
	UINT32 B_D;
	UINT32 G;
 
}SecDSignData;
 
INT32 TranslateHexStrToNum(char* Hex)
{
	char* hexString = Hex; // 要转换的十六进制字符串
 
	INT32 result;
	char* endptr;
 
	result = strtol(hexString, &endptr, 16); // 调用strtol函数进行转换
 
	return result;
}
 
string TranslateNumToHexStr(INT32 number)
{
	INT32 num = number; // 要转换的整型变量
 
	char hexString[100] = { 0 }; // 存放结果的字符串数组
 
	sprintf(hexString, "%X", num); // 使用%X格式化输入到字符串数组中
 
	return hexString;
}
 
int GetSecSignData(SecDSignData& out_res,
	char* in_SignLow, 
	char* in_SignHigh
)
{
	const INT32 signLow = TranslateHexStrToNum(in_SignLow);
	const INT32 signHigh = TranslateHexStrToNum(in_SignHigh);
 
	//得到端限长015位
	out_res.limit = signLow & 0xFFFF;
	//得到端限长1619位
	out_res.limit |= (signHigh & 0xF0000);
 
	//得到基地址015位
	out_res.base = (signLow & 0xFFFF0000) >> 16;
	//得到基地址1623位
	out_res.base |= (signHigh & 0xFF) << 16;
	//得到基地址2431位
	out_res.base |= (signHigh & 0xFF000000);
 
	out_res.TYPE = (signHigh & 0xF00) >> 8;
	//计算TYPE下的子属性
	{
		out_res.typedata.is = (signHigh & 0x800) >> 11;
		if (out_res.typedata.is)	//是 1，代码段
		{
			out_res.typedata.ci.c = (signHigh & 0x400) >> 10;
			out_res.typedata.ci.r = (signHigh & 0x200) >> 9;
		}
		else						//是0，数据段
		{
			//E
			out_res.typedata.di.e = (signHigh & 0x400) >> 10;
			//W
			out_res.typedata.di.w = (signHigh & 0x200) >> 9;
		}		
		out_res.typedata.a = (signHigh & 0x100) >> 8;
	}
 
	out_res.S = (signHigh & 0x1000) >> 12;
 
	out_res.DPL = (signHigh & 0x6000) >> 13;
 
	out_res.P = (signHigh & 8000) >> 15;
 
	out_res.AVL = (signHigh & 100000) >> 20;
 
	out_res.L = 0;
 
	out_res.B_D = (signHigh & 0x400000) >> 22;
 
	out_res.G = (signHigh & 0x800000) >> 23;
 
	return 0;
}
 
int print_result(SecDSignData in_print)
{
	cout << "limit(段限长/段界限): " 
		<< TranslateNumToHexStr(in_print.limit)	//十六进制
		<< endl;
 
	cout << "base(段基地址): "
		<< TranslateNumToHexStr(in_print.base)	//十六进制
		<< endl;
 
	cout << "TYPE(段类型): ";
 
	//开始输出TYPE的子属性
	if (in_print.typedata.is == 0)	//数据段
	{
		cout << "数据段" << endl;
		cout << '\t' << "TYPE:E: " << in_print.typedata.di.e << "    " << (in_print.typedata.di.e == 0 ? "向上扩展" : "向下扩展") << endl;
		cout << '\t' << "TYPE:W: " << in_print.typedata.di.w << "    " << (in_print.typedata.di.w == 0 ? "不可写" : "可写") << endl;		
	}
	else							//代码段
	{
		cout << "代码段" << endl;
		cout << '\t' << "TYPE:C: " << in_print.typedata.ci.c << "    " << (in_print.typedata.ci.c == 0 ? "非一致代码段" : "一致代码段") << endl;
		cout << '\t' << "TYPE:R: " << in_print.typedata.ci.r << "    " << (in_print.typedata.ci.r == 0 ? "不可读" : "可读") << endl;
	}
	//输出TYPE中的a位，看是否被访问过
	cout << '\t' << "TYPE:A: " << in_print.typedata.a << "    " << (in_print.typedata.a == 1 ? "被访问过" : "未被访问过") << endl;
 
	cout << "S(类型标志): "
		<< in_print.S
		<< '\t' << (in_print.S == 1 ? "存储段描述符(代码段或数据段)" : "系统描述符")
		<< endl;
 
	cout << "DPL(描述符特权级): "
		<< in_print.DPL
		<< endl;
 
	cout << "P(段存在标志): "
		<< in_print.P
		<< '\t' << (in_print.P == 1 ? "存在" : "不存在")
		<< endl;
 
	cout << "AVL(系统软件可用位,由操作系统来用): "
		<< in_print.AVL
		<< endl;
 
	cout << "L(保留位): "
		<< in_print.L
		<< endl;
 
	cout << "B/D(默认操作数大小): "
		<< in_print.B_D
		<< '\t' << (in_print.B_D == 0 ? "16位" : "32位")
		<< endl;
 
	cout << "G(颗粒度标志): "
		<< in_print.G
		<< '\t' << (in_print.G == 0 ? "单位:B" : "单位:4KB")
		<< endl;
 
	return 0;
}
 
// typedef unsigned char uint8_t;
// typedef unsigned short uint16_t;
// typedef unsigned int uint32_t;
/*================== GDT ==================*/

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

#pragma pack(1)
typedef struct gdt_table_t
{
    UINT16 limit_l;
    UINT16 base_l;
    UINT8 base_m;
    UINT16 attr;
    UINT8 base_h;
} gdt_table_t;

#pragma pack(1)
typedef struct gdt_gate_t
{
    UINT16 offset_l;
    UINT16 selector;
    UINT16 attr;
    UINT16 offset_h;
} gdt_gate_t;

/*==================GDT Functions==================*/


gdt_table_t gdt_table[GDT_SIZE] = {
    {0, 0, 0, 0}
};

void set_gdt_table_entry(int selector, UINT32 base, UINT32 limit, UINT16 attr)
{
    if (limit > 0xFFFFF) {
        attr |= 0x8000;
        limit >>= 12;
    }
    gdt_table_t *entry = &gdt_table[selector >> 3];
    entry->limit_l = limit & 0xFFFF;
    entry->base_l = base & 0xFFFF;
    entry->base_m = (base >> 16) & 0xFF;
    entry->attr = attr | (((limit >> 16) & 0xF) << 8);
    entry->base_h = (base >> 24) & 0xFF;
}

void init_gdt()
{
    for (int i = 0; i < GDT_SIZE; i++) {
        set_gdt_table_entry(i << 3, 0, 0, 0);
    }

	set_gdt_table_entry(0x8, 0x123456, 0xFFFFFFFF, 
        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_NORMAL | SEG_TYPE_DATA  | SEG_TYPE_RW | SEG_ATTR_D | SEG_ATTR_G);


    set_gdt_table_entry(0x10, 0x123456, 0xFFFFFFFF, 
        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_NORMAL | SEG_TYPE_DATA  | SEG_TYPE_RW | SEG_ATTR_D | SEG_ATTR_G);

    set_gdt_table_entry(0x20, 0, 0xFFFFFFFF, 
        SEG_ATTR_P | SEG_ATTR_DPL0 | SEG_NORMAL | SEG_TYPE_CODE  | SEG_TYPE_RW | SEG_ATTR_D | SEG_ATTR_G);

    set_gdt_table_entry(0x30, 0, 0xFFFFFFFF, 
        SEG_ATTR_P | SEG_ATTR_DPL3 | SEG_NORMAL | SEG_TYPE_DATA  | SEG_TYPE_RW | SEG_ATTR_D);
    set_gdt_table_entry(0x40, 0, 0xFFFFFFFF, 
        SEG_ATTR_P | SEG_ATTR_DPL3 | SEG_NORMAL | SEG_TYPE_CODE  | SEG_TYPE_RW | SEG_ATTR_D);
		
    //lgdt((uint32_t)gdt_table, (uint32_t)sizeof(gdt_table));
    // show_string("GDT init success!\r\n");
}

#define LOOPLINE while(1){  static int _i = 1; if(_i){_i=0;printf("LOOP _LINE_:%d\n",__LINE__);} }

void printHex3(unsigned int aa){
	printf("printHex3:%x\n",aa);
}

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
void parsegdt(){
	  // method 2
	//   std::string input="0x9a	0xcf	0x00	0xcc	0xff	0xff	0x00	0x00";
	//   input.replace("0x","");
	  
	  uint8_t tmparr[8]={0x9a,	0xcf	,0x00	,0xcc,	0xff,	0xff,	0x00	,0x00};
	//   memcpy(tmparr,&gdt_table[1],8);
	  gdt_table_t *pgdt =(gdt_table_t *)tmparr ; 
	  uint32_t src_baseadd = pgdt->base_l&0xFFFF;
	  src_baseadd = src_baseadd | (( pgdt->base_m&0xFF&0xFFFFFFFF) <<16 & 0xFFFFFFFF);
	  src_baseadd = src_baseadd | (( pgdt->base_h&0xFF&0xFFFFFFFF) <<24 & 0xFFFFFFFF);
  
	  printHex3(src_baseadd);
  
	  uint32_t  src_limit = (pgdt->attr & 0xFFFF)>>8 & 0x0F;
	  src_limit =    ((src_limit &0x0FFFFF) << 16) | (pgdt->limit_l&0xFFFF);
	  uint16_t g = pgdt->attr & (1<<15);
	  if(g){
		  src_limit = src_limit * 0x1000;
	  }

	 printHex3(src_limit);
  
	  //    printHex3(target_baseadd);
	  printf("xxxxxxxxxxx\n");
	  return ;
}

int main()
{
	init_gdt();
	parsegdt();
	return 0;
	for (size_t i = 0; i < 10; i++)
	{
		auto v =gdt_table[i];
		printf("%d:%X %X %X %X\t\n",
			i,v.base_h,v.base_m,v.base_l,v.limit_l);

		printf("%X\t",(unsigned char)(i<<3));
		unsigned char *s =(unsigned	char *)&v;
		for (size_t j = 0; j < sizeof(v); j++)
		{
			printf("%02X ",*s++);
		}
		printf("\t");
		s =(unsigned	char *)&v+sizeof(v)-1;
		for (size_t j = 0; j < sizeof(v); j++)
		{
			printf("%02X_",*s--);
		}

		printf("\n");
		
	}
	
	while (1)
	{
		char SecDescribeLow[100] = { 0 };
		char SecDescribeHigh[100] = { 0 };
		printf("please input xxx:\n");
		printf("输入段描述符的低32位:");
		scanf("%s", SecDescribeLow);
		printf("输入段描述符的高32位:");
		scanf("%s", SecDescribeHigh);
 
		SecDSignData info;
 
		// 00_00_92_0b_80_00_7f_ff
		//00_00_92_0b_80_00_7f_ff

		// 80_00_7f_ff  l 
		// 00_00_92_0b  g
		
		//80_00_7f_ff   ff7f0080
		//00_00_92_0b   0b920000

		//GetSecSignData(info, "12345678", "ABCDEF01");
		GetSecSignData(info, "80007fff", "0000920b");
		// GetSecSignData(info, "ff7f0080", "0b920000");
		// GetSecSignData(info, "0000920b", "80007fff");
		// GetSecSignData(info, SecDescribeLow, SecDescribeHigh);
 
		print_result(info);
 
		printf("\n");
	}
 
	return 0;
}