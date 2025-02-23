[org 0x7c00]

start:
a:
    mov si, msg
    call print
    xchg bx, bx
    xchg dx,dx
    ;jmp $
    xchg cx,cx
b:
    xchg dx,dx

    loop b
    xchg ax,ax
    
print:
    mov ah, 0x0e
    .nc:
        lodsb
        cmp al, 0
        je .done
        int 0x10
        jmp .nc
    .done:
        ret

msg db 'Hello,World!xxxa', 0

times 510 - ($ - $$) db 0
dw 0xaa55