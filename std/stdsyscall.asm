;; Made by Noah Van Miert
;; 9/12/2022
;; Apollo Compiler

;; This file will contain every syscall function (read, write, ...)
;; and soms handy macros (stdin, stdout, stderr, SYSCALL_WRITE, ...)

%define SYSCALL_WRITE 1

%define stdin  0
%define stdout 1
%define stderr 2


;; Write syscall - Writes a string to a specific file (stdout, stderr)
;; @rdi - File discriptor
;; @rsi - Buffer
;; @rdx - Buffer size
__sys_write:
    mov rax, SYSCALL_WRITE
    syscall
    ret