[bits 16]

_start:
    mov ax, 0
    mov es, ax
	mov ds, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
; 读取内核代码
read_disk:
	; ax=目标地址
	mov bx, 0x8000
	; ch=磁道号 cl=扇区号
	mov cx, 0x02
	; ah=2表示读扇区,3表示写扇区 al=读取/写入的扇区数
	mov ax, 0x240
	; dh=磁头号 dl=驱动器号(软驱从0开始, 硬盘从80H开始)
	mov dx, 0x80
	int 0x13
	jc read_disk
    xchg bx, bx
	jmp 0:0x8000	
	_testunuse:
push   ebp
 mov edx ,[ebp+0x10]
 mov edx ,[eax]
 mov [ebp+0x11] ,edx
 mov ebp ,esp
push   edi
push   esi
push   ebx
 sub esp ,0x210
 mov dword [ebp-0x1c] ,0x534d4150
 mov dword [ebp-0x20] ,0xe820
 mov dword [ebp-0x10] ,0x0
 mov dword [ebp-0x30] ,0x0
 lea ecx ,[ebp-0x210]
 mov edx ,[ebp-0x10]
 mov eax ,edx
 shl eax ,0x2
 add eax ,edx
 shl eax ,0x2
 add eax ,ecx
 mov [ebp-0x24] ,eax
 mov eax ,[ebp-0x20]
 mov ebx ,[ebp-0x10]
 mov ecx ,0x18
 mov edx ,[ebp-0x1c]
 mov esi ,[ebp-0x24]
 mov edi ,esi
int    0x15
 mov edx ,ebx
 mov [ebp-0x28] ,eax
 mov [ebp-0x2c] ,ecx
 mov [ebp-0x10] ,edx
 mov eax ,[ebp-0x30]
inc    eax
 mov [ebp-0x30] ,eax
 cmp dword [ebp-0x10] ,0x0
nop
 lea eax ,[ebp-0x210]
 mov [ebp-0x14] ,eax
 mov dword [ebp-0x18] ,0x0
 mov eax ,[ebp-0x14]
 lea edx ,[eax+0x1]
 mov [ebp-0x14] ,edx
 movzx   ecx ,byte [eax]
 mov eax ,[ebp+0x8]
 lea edx ,[eax+0x1]
 mov [ebp+0x8] ,edx
 mov dl ,cl
 mov [eax] ,dl
inc dword   [ebp-0x18]
 cmp dword [ebp-0x18] ,0x1e3
 xchg bx ,bx
nop
 add esp ,0x210
pop    ebx
pop    esi
pop    edi
pop    ebp
ret

times 510 - ($ - $$) db 0
dw 0xaa55