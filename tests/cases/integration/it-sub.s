//!args=--var
    .text
    .globl  f1
    .type   f1, @function
f1:
    mov %rdi,%rax
    sub $1,%rax
    sub $0x1234,%rax
    sub (%rsi),%rax
    ret
