//!args=--var
    .text
    .globl  f1
    .type   f1, @function
f1:
    mov %rdi,%rax
    add $1,%rax
    add $0x1234,%rax
    add (%rsi),%rax
    ret
