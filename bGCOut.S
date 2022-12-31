format elf64 executable 3

entry __main

segment readable executable

__exit:
mov rdi, [rsp + 8]   ; rsp + 8 contains the number we want
mov rax, 60          ; exit syscall number
syscall
; never returns, exit syscall



__printChar:
; move stack pointer into r14 so that we can add 8 without messing up the rest of the stack
mov r14, rsp
; skip return address, 8 bytes
add r14, 8
; syscall setup
mov rax, 1           ; write syscall number
mov rdi, 1           ; stdout file descriptor
mov QWORD rsi, r14   ; r14 has the pointer to our character
mov rdx, 1           ; only one character to be printed
syscall
ret
segment readable executable



__main:
push QWORD 65
push QWORD 1
push QWORD 1
push QWORD 1
; TOK_equals
pop r15
pop r14
sub r14, r15
jz __L0
push QWORD 0
jmp __L1
__L0:
push QWORD 1
__L1:
; TOK_equals
pop r15
pop r14
sub r14, r15
jz __L2
push QWORD 0
jmp __L3
__L2:
push QWORD 1
__L3:
; TOK_add
pop r15
pop r14
add r14, r15
push r14
call __printChar
add rsp, 8
push QWORD 0
call __exit
add rsp, 8
