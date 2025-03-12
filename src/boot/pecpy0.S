.extern pememcpy

.code32
.text
.global _start
_start:
	mov $0x3, %ax
	a:
	nop
	//jmp a
	call pememcpy
	xchg %bx, %bx
	nop 
	nop
	nop
	retf

	//int $0x10
	//jmp $KERNEL_CODE_SEG, $kernel32_init
	