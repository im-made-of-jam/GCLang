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
