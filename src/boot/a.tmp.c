push   %ebp
mov    %dl,(%eax)
mov    0x10(%ebp),%edx
mov    (%eax),%edx
mov    %edx,0x11(%ebp)
mov    %esp,%ebp
push   %edi
push   %esi
push   %ebx
sub    $0x210,%esp
movl   $0x534d4150,-0x1c(%ebp)
movl   $0xe820,-0x20(%ebp)
movl   $0x0,-0x10(%ebp)
movl   $0x0,-0x30(%ebp)
lea    -0x210(%ebp),%ecx
mov    -0x10(%ebp),%edx
mov    %edx,%eax
shl    $0x2,%eax
add    %edx,%eax
shl    $0x2,%eax
add    %ecx,%eax
mov    %eax,-0x24(%ebp)
mov    -0x20(%ebp),%eax
mov    -0x10(%ebp),%ebx
mov    $0x18,%ecx
mov    -0x1c(%ebp),%edx
mov    -0x24(%ebp),%esi
mov    %esi,%edi
int    $0x15
mov    %ebx,%edx
mov    %eax,-0x28(%ebp)
mov    %ecx,-0x2c(%ebp)
mov    %edx,-0x10(%ebp)
mov    -0x30(%ebp),%eax
inc    %eax
mov    %eax,-0x30(%ebp)
cmpl   $0x0,-0x10(%ebp)
je     7c7 <_memory_check+0x6f>
jmp    780 <_memory_check+0x28>
nop
lea    -0x210(%ebp),%eax
mov    %eax,-0x14(%ebp)
movl   $0x0,-0x18(%ebp)
jmp    7f6 <_memory_check+0x9e>
mov    -0x14(%ebp),%eax
lea    0x1(%eax),%edx
mov    %edx,-0x14(%ebp)
movzbl (%eax),%ecx
mov    0x8(%ebp),%eax
lea    0x1(%eax),%edx
mov    %edx,+0x8(%ebp)
mov    %cl,%dl
mov    %dl,(%eax)
incl   -0x18(%ebp)
cmpl   $0x1e3,-0x18(%ebp)
jbe    7da <_memory_check+0x82>
xchg   %bx,%bx
nop
add    $0x210,%esp
pop    %ebx
pop    %esi
pop    %edi
pop    %ebp
ret