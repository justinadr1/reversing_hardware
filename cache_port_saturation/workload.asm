; workload.asm
bits 64
section .data
    align 64
    dummy_buffer times 1024 dq 0

section .text
global workload

workload:
    mov rcx, 50000000            
    lea rdx, [rel dummy_buffer] 

.loop:
    mov qword [rdx], rax         
    mov rbx, qword [rdx]         
    add rax, 1                   
    dec rcx
    jnz .loop
    ret