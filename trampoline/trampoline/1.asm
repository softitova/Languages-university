USE64
lbl:
cmp     rax, rsp
je      lbl2
mov     rdi, [rsp]
mov     [rsp-0x8],rdi
jmp     lbl
lbl2:
