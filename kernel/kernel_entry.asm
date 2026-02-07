[BITS 64]
[EXTERN kernel_main]

global _start

section .text
_start:
    ; Initialiser le stack
    mov rsp, stack_top
    
    ; Nettoyer les registres
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor rbp, rbp
    
    ; Appeler le kernel principal en C
    call kernel_main
    
    ; Si le kernel retourne, arrêter
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB de stack
stack_top:
