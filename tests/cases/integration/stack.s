//!args=--run -5
        .intel_syntax noprefix
        .text
        .globl  f1
        .type   f1, @function
f1:
	push rbp
	mov rbp, rsp
	push 1
	push rdi
	push rsi
	push 2
	mov rax, [rbp-8]
	add rax, [rbp-16]
	pop rbx
	add rax, rbx
	mov rsp, rbp
	pop rbp
	ret
