section .multiboot_header
align 8
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

align 8
    dw 0
    dw 0
    dd 8
header_end:

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
bits 32

global _start
extern kernel_main

_start:
    mov esp, stack_top
    
    call kernel_main
    
    cli
.hang:
    hlt
    jmp .hang