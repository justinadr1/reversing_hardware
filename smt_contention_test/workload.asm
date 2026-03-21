; workload.asm
bits 64
section .text
global workload

workload:
    mov rcx, 100'000'000
    mov rax, 0x12345678
    mov rbx, 7          

.loop:
    xor rdx, rdx        
    idiv rbx            
    add rax, 1          
    dec rcx
    jnz .loop

    ret