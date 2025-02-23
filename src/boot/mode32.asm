;mode32.asm
 
;[org 0x800]
%ifndef _nod_org
    [org 0x800]
%endif
[section .text]
[bits 32]
;xchg bx, bx
mode32:
    mov ax, cs
    mov gs, ax
    mov si, M_32

    call print32

    ;hlt
    xchg bx, bx
    mov ax,0x50
    mov gs,ax
    mov eax,0x40000000
    mov ebx, [gs:eax]
    mov edx, 0x123
    mov  [gs:eax],edx
    mov ecx, [gs:eax]
    ;mov ecx,0x5623
    mov esi,0x300000
    mov dword [gs:esi],0x123
    mov esi,0x300004
    mov dword [gs:esi],ecx
    a:
    nop
    nop
    ;jmp a 
    mov ax,0x20
    mov ds,ax
    call 0x18:0x1400 ; pecpy_start

    xchg bx, bx
    nop
    nop
    ;call  0x28:0x41103c
    call 0x28:callfmain3
    nop
    nop
    mov ax,0x10
    mov ds,ax
    xchg bx, bx
    nop
    nop
    call 0x8:callfkernel32_start
    nop
    nop

callfkernel32_start:
    mov eax,0x53D 
    push eax       ; gdt
    mov eax,0x200 
    push eax       ; mem_info
    call 0x11000
    nop
    nop
    retf

callfmain3:
    mov ax,0x30
    mov ds,ax
    mov eax,0
    push eax ;9
    push eax ;8
    mov eax,0x50000000 ;3fc01000
    push eax
    mov eax,0x010000   ;6
    push eax
    mov eax,400 ; 5
    push eax
    mov eax,0x200000
    push eax
    mov eax,0
    push eax  
    mov eax,0
    push eax
    mov eax,1
    push eax  ; 1
    ;main3(UINT32 __model, void* __vprintfFunc, void* __VirtualProtectFunc,
	;char* buffer, size_t buffer_len,     5
	;UINT32 img_load_addr, size_t img_len,  6 7
	;UINT32 addr_off, UINT32* pparam)
    ;call  0x28:0x41103c
    call 0x41e5a0   
    xchg bx, bx
    add esp,0x24
    nop
    nop
    nop
    retf

print32:
    mov ax, 0x40
    mov ds, ax
    mov di, 0x140
    mov ah, 0x62
.loop:
    ;mov al, [gs:si-0x800]
    mov al, [es:si]
    cmp al, 0
    jz .done
    mov word[ds:di], ax
    inc si
    inc di
    inc di
    jmp .loop
.done:
    ret
M_32:
    db "32 bit routine ...", 0