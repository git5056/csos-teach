.text
.global _start
.extern csos_init
_start:
    push %ebp
    mov %esp, %ebp
    mov 0xC(%ebp), %eax
    push %eax
    mov 0x8(%ebp), %eax
    push %eax
    call csos_init
    
    jmp .

.text
.global _rdmsr
_rdmsr:
    pusha
    mov $0x1B,%ecx
    rdmsr
    //rdmsr
    //ABC equ 0xFEE000F0
    mov (0xFEE000F0),%eax
    or $0x100,%eax
    mov %eax,(0xFEE000F0)
    popa
    ret

.text
.global _start_multi_core
.extern simple_sleep
_start_multi_core:
//    ; 向所有其他 AP 发送 INIT
    pusha
    //cli
    mov $0x000C4500,%eax //  ; Load ICR encoding for broadcast INIT IPI to all APs into EAX.
    mov $0xFEE00300 ,%esi
    mov %eax ,(%esi)

call simple_sleep

    mov (%esi),%ecx
 //; 向所有其他 AP 发送 SIPI
    mov  $0x000C4608,%eax
    mov %eax,( %esi )
qww:
jmp qww
   pusha
    call simple_sleep
    popa
    mov (%esi),%ebx
    //; 向所有其他 AP 发送 SIPI
    mov  $0x000C4608,%eax
    mov %eax,( %esi )

    popa

    ret

.text
.global simple_switch
simple_switch:
    mov 4(%esp), %ecx
    mov 8(%esp), %edx
    push %ebp
    push %ebx
    push %esi
    push %edi
    pushf
    mov %esp, (%ecx)
    mov %edx, %esp
    popf
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    ret

.global syscall_handler
.extern syscall
syscall_handler:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    pushf

    mov %esp, %eax
    push %eax
    call syscall
    add $4, %esp

    popf
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    retf $(5 * 4)

.macro interrupt_handler func vector code
.text
.global interrupt_handler_\func
.extern handler_\func
interrupt_handler_\func:
    .if \code == 0
        push $0
    .endif
    push $\vector

    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    push %esp
    call handler_\func
    add $(1 * 4), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2 * 4), %esp

    iret

.endm



interrupt_handler default,          -1,     0
interrupt_handler division,         0,      0
interrupt_handler debug,            1,      0
interrupt_handler nmi,              2,      0
interrupt_handler breakpoint,       3,      0
interrupt_handler overflow,         4,      0
interrupt_handler range,            5,      0
interrupt_handler opcode,           6,      0
interrupt_handler device,           7,      0
interrupt_handler double,           8,      1
interrupt_handler tss,              10,     1
interrupt_handler segment,          11,     1
interrupt_handler stack,            12,     1
interrupt_handler protection,       13,     1
interrupt_handler page,             14,     1
interrupt_handler fpu,              16,     0
interrupt_handler align,            17,     1
interrupt_handler machine,          18,     0
interrupt_handler simd,             19,     0
interrupt_handler virtual,          20,     0
interrupt_handler control,          21,     1

.extern  default_task_entry2
.text 
.global xxxfunc
xxxfunc:
mov $1,%eax
asdaq:
nop
//    mov (0x1310e),%eax
lea default_task_entry2,%eax
//jmp asdaq
call %eax
call default_task_entry2

//interrupt_handler timer,            0x20,   0

.extern isNeedSW
.extern switch_task

.extern reteipaddr
.text
.global switch_task_asm
switch_task_asm:
//sub $0x10,%esp
sub $4,%esp
pushf
pusha
push %eax
call switch_task
//mov %eax,0x4(%esp)
mov %eax,0x28(%esp)
//push %ss
//pop %ax
//mov %ax,-8(%esp)
//add $0x10,%esp
pop %eax
//mov $0x12312123,%eax
//mov $0x10123121,%ebx
//mov $0x112312323,%ecx
popa
popf
//sub $4,%esp
//iret
ret


.text
.global interrupt_handler_timer
.extern handler_timer
interrupt_handler_timer:
    push $0 
    push $0x20

    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    push %esp
    call handler_timer
    add $(1 * 4), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2 * 4), %esp

    cmpl $0x1,isNeedSW
    jne xxx100
    incl isNeedSW
push %eax
    movl 4+(%esp),%eax
    movl %eax,reteipaddr
    lea   switch_task_asm,%eax
    movl %eax,4+(%esp)
pop %eax
    jmp xxx100

    sub $0xc,%esp
    pusha


    .set xxxdis, 0x20

    .macro xxxdis2
     0x123
     mov %ecx,0x0+ $xxxdis (%esp)
    .endm

    cmpl $0x1,isNeedSW
    jne xxx1
    incl isNeedSW
    mov 4+0xc+xxxdis(%esp),%eax
    mov 8+0xc+xxxdis(%esp),%ecx
    lea switch_task_asm,%edx
    //pushl switch_task_asm
    mov %edx,0x0 +xxxdis (%esp)
    mov %eax,0x4 +xxxdis (%esp)
    mov %ecx,0x8 +xxxdis (%esp)
    popa
    jmp xxx2
        
xxx1:
    popa
    add $0xc,%esp   

 xxx2:    
xxx100:
    iret
interrupt_handler kbd,              0x21,   0
interrupt_handler rtc,              0x28,   0
interrupt_handler hdc,              0x2E,   0

interrupt_handler syscall,          0x80,   0