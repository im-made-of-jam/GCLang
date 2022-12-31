format elf64 executable 3

entry __main

; [rsp + 8] is often used in this due to the return address being at the top of the stack
; this means we cannot simply pop rxx as this would pop the return address instead of what we want
; as such, we add 8 bytes to where we access the information, and then remove it from the stack after returning

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


__sleepms:
; rsi holds remaining time of sleep if it was interrupted
; we dont care, so we set it to null to signal that fact to the kernel
mov rsi, 0
; we get the time in ms, but the kernel takes nanoseconds
; thus we multiply by 1 000 000
mov rax, [rsp + 8]
xor rdx, rdx
mov rbx, 1000000
mul rbx
mov [__sleepms_req + 8], rax
mov rdi, __sleepms_req
mov rax, 35 ; nanosleep syscall
syscall
ret

segment readable writeable

__sleepms_req dq 0, 0
segment readable executable



__main:

; auto-generated code below, reading it ay be difficult

; TOK_num_combo
mov rax, 0
push rax

; TOK_if_begin
pop r15
cmp r15, 0
jz __L2

; TOK_num_combo
mov rax, 1
push rax

; TOK_call_extern
call __exit
add rsp, 8

; TOK_if_else
jmp __L1
__L2:

; TOK_num_combo
mov rax, 0
push rax

; TOK_if_begin
pop r15
cmp r15, 0
jz __L4

; TOK_num_combo
mov rax, 2
push rax

; TOK_call_extern
call __exit
add rsp, 8

; TOK_if_end
__L4:
__L3:

; TOK_if_end
__L1:
mov rax, 60
mov rdi, 0
syscall

;
