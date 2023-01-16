; [rsp + 8] is often used in this due to the return address being at the top of the stack
; this means we cannot simply pop rxx as this would pop the return address instead of what we want
; as such, we add 8 bytes to where we access the information, and then remove it from the stack after returning

segment readable executable

; stack size is quite small by default, about 8MiB
; this is nothing like enough for a stack based language, so we should increase it
; this does just that
__setStackSize:
mov rax, 97                           ; getrlimit syscall number
mov rdi, 3                            ; 3 is RLIMIT_STACK, which tells the kernel we want to get the size of the stack
mov rsi, __setStackSize_rlimit_struct ; pointer to rlimit struct
syscall
push rax
ret


; now because of the way that code is generated we need to expand the stack then add the return value to the top again
pop r15  ; return address
push rax ; opened file descriptor
push 0   ; this will be removed from the stack immediately after the function returns, leaving us with the value from rax left on the top of the stack
push r15 ; return address
ret


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
mov rax, 1   ; write syscall number
mov rdi, 1   ; stdout file descriptor
mov rsi, r14 ; r14 has the pointer to our character
mov rdx, 1   ; only one character to be printed
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
mov [__sleepms_requested + 8], rax
mov rdi, __sleepms_requested
mov rax, 35 ; nanosleep syscall
syscall
ret


__sleepus:
; rsi holds remaining time to sleep if the syscall is interrupted
; we dont care, so we set it to null to signal that fact to the kernel
mov rsi, 0
; we get the time in us, but the kernel takes nanoseconds
; thus we multiply by 1 000
mov rax, [rsp + 8]
xor rdx, rdx
mov rbx, 1000
mul rbx
mov [__sleepms_requested + 8], rax
mov rdi, __sleepms_requested
mov rax, 35 ; nanosleep syscall
syscall
ret


__openFile:
; rdi has filename pointer
; rsi has flags
; rdx has open mode
mov rdi, __openFile_filename
; O_RDONLY is defined as 0, we only want to read from this file so this is fine for now
xor rsi, rsi
; mode applies to creating files, so we have 0
xor rdi, rdi
syscall
; now because of the way that code is generated we need to expand the stack then add the return value to the top again
pop r15  ; return address
push rax ; opened file descriptor
push 0   ; this will be removed from the stack immediately after the function returns, leaving us with the value from rax left on the top of the stack
push r15 ; return address
ret



__printI:
mov rax, [rsp + 8]
; first we need to get the number from integer representation to string representation
; we are dividing by 10, so we have 10 in rbx to use as the divisor
mov rbx, 10
; 20 characters is the maximum number of base 10 characters contained in 64 bits, therefore a 20 character loop
mov rcx, 20

.__printI_loop_begin:
; set up the division: rdx = 0
xor rdx, rdx
div rbx
; rdx has the mod value, which is the character we need
; ascii numbers begin at value 48, so we add that to rdx
add rdx, 48
; now we have the ascii character we want, so we can put it into the memory location we need
; check for early termination
mov [__printI_string + rcx - 1], dl ; -1 for indexing reasons
cmp rax, 0
je __printI.__printI_loop_finish
; check for all characters used up
dec rcx
cmp rcx, 0
jne __printI.__printI_loop_begin

.__printI_loop_finish:
; r15 will contain pointer to string
mov r15, __printI_string
add r15, rcx
sub r15, 1
; r14 has 20 - (number of characters)
mov r14, rcx
; now (number of characters) - 20
neg r14
mov rcx, 20
;    20 + (number of characters) - 20
; == number of characters
add rcx, r14
; indexing fun
add rcx, 1

; now we write the number to stdout
mov rax, 1               ; write syscall
mov rdi, 1               ; stdout
mov rsi, r15             ; character buffer pointer
mov rdx, rcx             ; number of characters
syscall
ret






segment readable writeable

__setStackSize_rlimit_struct dq 0, 0

__sleepms_requested dq 0, 0

__openFile_filename db "./GCIn.txt", 0
__openFile_fd dq 0

__printI_string db "18446744073709551615"