_memory_check:
    push   ebp
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
je     7c7 <_memory_check+0x6f>
jmp    780 <_memory_check+0x28>
nop
	lea eax ,[ebp-0x210]
	mov [ebp-0x14] ,eax
	mov dword [ebp-0x18] ,0x0
jmp    7f6 <_memory_check+0x9e>
	mov eax ,[ebp-0x14]
	lea edx ,0x1(eax)
	mov [ebp-0x14] ,edx
	movzbl ecx ,(eax)
	mov eax ,0x8(ebp)
	lea edx ,0x1(eax)
	mov 0x8(ebp) ,edx
	mov dl ,cl
	mov (eax) ,dl
incl   [ebp-0x18]
	cmp dword [ebp-0x18] ,0x1e3
jbe    7da <_memory_check+0x82>
	xchg bx ,bx
nop
	add esp ,0x210
pop    ebx
pop    esi
pop    edi
pop    ebp
ret123123235