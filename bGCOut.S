format elf64 executable 3

entry __main

segment readable executable

__exit:
mov rdi, [rsp + 8]   ; rsp + 8 contains the number we want
mov rax, 60          ; exit syscall number
syscall
; never returns, exit syscall


__printChar:
; preserve r14
push r14
; move stack pointer into r14 so that we can add 8 without messing up the rest of the stack
mov r14, rsp
; skip return address and r14, both 8 bytes, 16 combined
add r14, 16
; syscall setup
mov rax, 1           ; write syscall number
mov rdi, 1           ; stdout file descriptor
mov QWORD rsi, r14   ; r14 has the pointer to our character
mov rdx, 1           ; only one character to be printed
syscall
; preserve r14
pop r14
ret
segment readable executable
__main:



push 97
call __printChar
push 10
call __printChar
push 0
call __exit
