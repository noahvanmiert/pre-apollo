;; Made by Noah Van Miert
;; 9/12/2022
;; Apollo Compiler

;; This file will contain every syscall function (read, write, ...)
;; and soms handy macros (stdin, stdout, stderr, SYSCALL_WRITE, ...)


%ifndef __STDSYSCALL__
%define __STDSYSCALL__

%define SYSCALL_READ  0
%define SYSCALL_WRITE 1
%define SYSCALL_OPEN  2
%define SYSCALL_CLOSE 3
%define SYSCALL_EXIT  60

%define stdin  0
%define stdout 1
%define stderr 2


;; syscall read - Reads from a specific file (stdin) and store it in a given buffer
;; @rdi - File discriptor (unsigned int)
;; @rsi - Buffer buffer (char *)
;; @rdx - Count (size_t)
__sys_read:
    mov rax, SYSCALL_READ
    syscall
    ret


;; syscall write - Writes a buffer to a specific file (stdout, stderr)
;; @rdi - File discriptor (unsigned int)
;; @rsi - Buffer (const char *)
;; @rdx - Count (size_t)
__sys_write:
    mov rax, SYSCALL_WRITE
    syscall
    ret


;; syscall open - Opens a file 
;; @rdi - Filename (const char *)
;; @rsi - Flags (int)
;; @rdx - Mode (unsigned short)
__sys_open:
    mov rax, SYSCALL_OPEN
    syscall
    ret


;; syscall close - Closes a file
;; @rdi - File descriptor (unsigned int)
__sys_close:
    mov rax, SYSCALL_CLOSE
    syscall
    ret


;; syscall exit - Terminates the current process immediately
;; @rdi - Exit code
__sys_exit:
    mov rax, SYSCALL_EXIT
    syscall
    ret


; __STDSYSCALL__
%endif