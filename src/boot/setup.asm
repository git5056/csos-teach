;setup.asm
 
;[org 0x500]
%ifndef _nod_org
    [org 0x500]
%endif
[section .text]
[bits 16]
 
;xchg bx, bx
    mov si, M_runSetup
    call print

 ;xchg bx,bx
    cli
    lgdt [gdt_entry]

    mov dx, 0x92
    in al, dx
    or al, 0x2
    out dx, al
 
    mov eax, cr0
    or  eax, 0x1
    mov cr0, eax
       ;jmp $
xchg bx, bx
    jmp 0x38:0
 
print:
    mov ax, 0xb800
    mov gs, ax
    mov di, 0xa0
    mov ah, 0x26
.loop:
    mov al, [si]
    cmp al, 0
    jz .done
    mov word[gs:di], ax
    inc si
    inc di
    inc di
    jmp .loop
.done:
    ret
 
GDT:
    dq 0
    dq 0x_00_CF_9A_00_00_00_FF_FF ; 32 code segment  8h kernel_code
    dq 0x_00_cf_92_00_00_00_ff_ff ; 32 data segment  10h kernel_data
   ;dq 0x_00_00_92_00_80_00_7f_ff ; display memory area
    dq 0x_00_40_9e_00_00_00_71_ff ; 32 code segment  18h pecpy_code
    dq 0x_00_cf_92_00_00_00_ff_ff ; 32 data segment  20h pecpy_data
    dq 0x_00_CF_9A_00_00_00_FF_FF ; 32 code segment  28h peloader_code
    dq 0x_00_cf_92_00_00_00_ff_ff ; 32 data segment  30h peloader_data
    dq 0x_00_40_9e_00_08_00_76_ff ; 32 code segment  0x38 mode32_code
    dq 0x_00_00_92_0b_80_00_7f_ff ; 16 display memory area 0x40 mode32_data
    dq 0
    dq 0x_00_cf_92_00_00_00_ff_ff ; 32 data segment 

END:

gdt_len equ END-GDT-1
 
gdt_entry:
    dw gdt_len
    dd GDT
 
M_runSetup:
    db "Setup routing running ...", 0