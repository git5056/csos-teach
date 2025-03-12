;nasm testap.asm -o testap.bin       
;qemu-system-x86_64 -smp 4 -m 1M -drive file=test.bin,format=raw  

APIC_ID equ 0xFEE00020 ; Local APIC ID Register
APIC_SVR equ 0xFEE000F0 ; Spurious Interrupt Vector Register
APIC_ICR_LOW equ 0xFEE00300 ; Interrupt Command Register

AP_START_ADDR equ 0x00008000 ; 该地址需要为 0x000XX000 的形式

    ;org 0x7C00 ; 引导扇区会被加载到内存 0x7C00 处
%ifndef _nod_org
    [org 0x7c00]
%endif
[bits 16] ; BSP 引导时处于实模式

    cli
    jmp 0x0000:bspStart

bspStart:
    ; 进入保护模式，不然我们访问不到 0xFEE00XXX 处的 Local APIC 寄存器
    mov ax, 0x0000
    mov ds, ax
    lgdt [GDT_DESC]
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    jmp dword GDT_CODE - GDT:bspProtect

[bits 32]

bspProtect:
    mov ax, GDT_DATA - GDT
    mov ds, ax
    mov es, ax

    ; 将 apStart 部分代码复制到 AP_START_ADDR
    mov esi, apStart
    mov edi, AP_START_ADDR
    mov ecx, apStartEnd - apStart
    cld
    rep movsb

    ; 启用 APIC
    mov eax, [APIC_SVR]
    or eax, 0x00000100 ; APIC Software Enable/Disable = 1
    mov [APIC_SVR], eax
    ; 向所有其他 AP 发送 INIT
    mov eax, 0x000C4500
    mov [APIC_ICR_LOW], eax
    ; 延时，简单起见直接使用了一个空循环，下同
    mov ecx, 100000000
    zxc:
    nop
    ; loop zxc    ;实在是想不明白gdb的实现原理,不知道为啥只要下了断点，每次执行这个循环就极其慢,
    ; 导致卡在这里，即使我没有在此处下断，而是在当前文件其他任意位置下断也会导致这个情况
    ;loop $
    ; 向所有其他 AP 发送 SIPI
    mov eax, 0x000C4600 | (AP_START_ADDR >> 12)
    mov [APIC_ICR_LOW], eax
    ; 延时
    mov ecx, 100000000
    ;loop $       ;注释掉，原因同上

    ; 获取 Local APIC ID
    mov ebx, [APIC_ID]
    shr ebx, 24
    ; 在屏幕上绘制绿底白字的字符
    mov edi, 0x000B8A00
    mov eax, ebx
    mov cl, 10
    div cl
    add ah, '0'
    mov [edi + 2 * ebx], ah ; 显示字符为 Local APIC ID % 10
    mov byte [edi + 2 * ebx + 1], 0x2F

    ; 停机
    hlt
    jmp $

[bits 16] ; AP 唤醒时处于实模式

apStart:
    jmp 0x0000:apMain
apStartEnd:

apMain:
    ; 进入保护模式
    mov ax, 0x0000
    mov ds, ax
    lgdt [GDT_DESC]
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    jmp dword GDT_CODE - GDT:apProtect

[bits 32]

apProtect:
    mov ax, GDT_DATA - GDT
    mov ds, ax

    ; 获取 Local APIC ID
    mov ebx, [APIC_ID]
    shr ebx, 24
    ; 在屏幕上绘制蓝底白字的字符
    mov edi, 0x000B8A00
    mov eax, ebx
    mov cl, 10
    div cl
    add ah, '0'
    mov [edi + 2 * ebx], ah ; 显示字符为 Local APIC ID % 10
    mov byte [edi + 2 * ebx + 1], 0x1F

    ; 停机
    hlt
    jmp $

GDT:
    dq 0x0000000000000000
GDT_CODE: ; 平坦代码段
    dq 0x00CF9A000000FFFF
GDT_DATA: ; 平坦数据段
    dq 0x00CF92000000FFFF
GDT_END:

GDT_DESC:
    dw GDT_END - GDT - 1
    dd GDT

    times 510 - ($ - $$) db 0x00
    db 0x55, 0xAA ; 可引导标识