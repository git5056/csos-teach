; boot2.asm
;[org 0x7c00]
%ifndef _nod_org
    [org 0x7c00]
%endif
[section .data]
LSA equ 0x500
M32 equ 0x800
PECPY equ 0x1000

[section .text]
[bits 16]
global _start
global main
main:
_start:
    xchg bx, bx
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x1000
    mov ecx, 0x1B
rdmsr
mov eax,1
    cpuid
jmp zxc
mov eax,1
    cpuid
        and ebx, 0xff000000
    shr ebx, 24 
    and eax,0xff
    mov eax,ebx
    cmp eax,0
    je zxc
  asf: 
  nop
  nop 
    jmp asf 
zxc:
    mov ax, 3
    int 0x10
 
    mov si, M_ldSetup
    call print
 
    mov al, 2
    mov bl, 1  ;setup routine load to 2nd sector
    mov si, LSA;memory address
    call readHD
 
    mov al, 2
    mov bl, 2  ;mode32 routine load to 3rd sector
    mov si, M32;memory address
    call readHD
 
    ; ax=目标地址
	mov bx, PECPY
	; ch=磁道号 cl=扇区号
	mov cx, 0x04
	; ah=2表示读扇区,3表示写扇区 al=读取/写入的扇区数
	mov ax, 0x215
	; dh=磁头号 dl=驱动器号(软驱从0开始, 硬盘从80H开始)
	mov dx, 0x80
	int 0x13

    ;xchg bx, bx
    xchg eax,eax
    xchg ecx,ecx
    call _memory_check
    ;xchg bx, bx

    jmp  LSA
 
readHD:
    mov dx, 0x1f2 ;count
    mov al, 2
    out dx, al
 
    inc dx        ;LBA address
    mov al, bl
    out dx, al
 
    inc dx
    xor al, al
    out dx, al
 
    inc dx
    out dx, al
 
    mov dx, 0x1f6
    mov al, 0xe0
    out dx, al
 
    mov dx, 0x1f7
.waitRDY:
    in al, dx
    test al, 0x40
    jz .waitRDY
 
    mov al, 0x20
    out dx, al
 
.waitDRQ:
    in al, dx
    test al, 0x08
    jz .waitDRQ
 
    mov cx, 0x100 ;256
    mov dx, 0x1f0
    mov di, si
.readDAT:
    in ax, dx
    push dx
    push ax
    mov dx, 0x1f7
    in al, dx
    test al, 0x01 ;ERR
    jnz .done
    pop ax
    pop dx
    mov word[di], ax
    add di, 2
    loop .readDAT
.done:
    ret
 
print:
    mov ah, 0xe
    mov bh, 0
    mov bl, 1
.loop:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .loop
.done:
    ret

_memory_check:
    push   ebp
    mov ebp ,esp
    push   edi
    push   esi
    push   ebx
    sub esp ,0x200
    mov dword [ebp-0x14] ,0x534d4150
    mov dword [ebp-0x18] ,0xe820
    mov dword [ebp-0x10] ,0x0
    mov dword [ebp-0x28] ,0x0
._memory_check_780:
    lea ecx ,[ebp-0x208]
    mov edx ,[ebp-0x10]
    mov eax ,edx
    shl eax ,0x2
    add eax ,edx
    shl eax ,0x2
    add eax ,ecx
    mov [ebp-0x1c] ,eax
    mov eax ,[ebp-0x18]
    mov ebx ,[ebp-0x10]
    mov ecx ,0x18
    mov edx ,[ebp-0x14]
    mov esi ,[ebp-0x1c]
    mov edi ,esi
    int    0x15
    mov edx ,ebx
    mov [ebp-0x20] ,eax
    mov [ebp-0x24] ,ecx
    mov [ebp-0x10] ,edx
    mov eax ,[ebp-0x28]
    inc    eax
    mov [ebp-0x28] ,eax
    cmp dword [ebp-0x10] ,0x0
    je   ._memory_check_7c7 ;7c7 ;<_memory_check+0x6f>
    jmp  ._memory_check_780 ;780 ;<_memory_check+0x28>
._memory_check_7c7:
    nop
xchg bx ,bx
    nop

;mov ax,0
;mov gs,ax
mov cx,500
lea si, [ebp-0x208]  ; Load source address into SI
lea di, 0x200    ; Load destination address into DI
cld
rep movsb 

    add esp ,0x200
    pop    ebx
    pop    esi
    pop    edi
    pop    ebp
    ret

M_ldSetup:
    db "Load setup routine ...", 13, 10, 0
 
times 510 - ($-$$) db 0
db 0x55, 0xaa



times 1024 - ($-_start) db 0

multi_core_boot:    ; 0x8000开始
    ;jmp 0x0000:ap_main
ap_main:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov eax,0x12345678
    ;mov gs:[0x7c00],eax
    hlt
    jmp $