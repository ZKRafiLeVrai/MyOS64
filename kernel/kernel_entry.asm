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

align 4096
pml4_table:
    resb 4096
pdp_table:
    resb 4096
pd_table:
    resb 4096

section .data
align 16
gdt64:
    dq 0
    dq 0x00209A0000000000  ; Code: exec/read, 64-bit
    dq 0x0000920000000000  ; Data: read/write
gdt64_pointer:
    dw 23
    dq gdt64

section .text
bits 32
global _start
extern kernel_main

_start:
    cli
    mov esp, stack_top
    
    ; Clear page tables
    mov edi, pml4_table
    mov ecx, 3072
    xor eax, eax
    rep stosd
    
    ; Setup page tables
    mov eax, pdp_table
    or eax, 3
    mov [pml4_table], eax
    
    mov eax, pd_table
    or eax, 3
    mov [pdp_table], eax
    
    mov eax, 0x83
    mov [pd_table], eax
    
    ; Enable PAE
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    
    ; Load PML4
    mov eax, pml4_table
    mov cr3, eax
    
    ; Enable Long Mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    
    ; Load GDT and jump to 64-bit
    lgdt [gdt64_pointer]
    jmp 0x08:long_mode

bits 64
long_mode:
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov rsp, stack_top
    call kernel_main
    
.hang:
    cli
    hlt
    jmp .hang