segment .text
global _start:
_start:
	call main
	mov rax, 60
	mov rdi, 0
	syscall

test:
	push rbp
	mov rbp, rsp
	mov rdi, 1
	mov rsi, str_0
	mov rdx, 13
	mov rax, 1
	syscall
	mov rdi, 1
	mov rax, 60
	syscall
	pop rbp
	ret

main:
	push rbp
	mov rbp, rsp
	call test
	pop rbp
	ret

segment .data
str_0: db "Hello, World"
