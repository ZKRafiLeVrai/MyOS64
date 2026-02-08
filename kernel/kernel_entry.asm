bits 32

section .multiboot_header
align 8
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
    dw 0, 0, 8
header_end:

section .bss
align 4096
pml4_table: resb 4096
pdp_table:  resb 4096
pd_table:   resb 4096
align 16
stack_bottom: resb 16384
stack_top:

section .data
align 16
gdt64:
    dq 0 ; null
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
global _start
extern kernel_main

_start:
    cli
    mov esp, stack_top

    ; --- NETTOYAGE DES TABLES (CRITIQUE RYZEN) ---
    mov edi, pml4_table
    xor eax, eax
    mov ecx, 3072 ; Nettoie PML4, PDP et PD
    rep stosd

    ; Setup des tables
    mov eax, pdp_table
    or eax, 0b11
    mov [pml4_table], eax

    mov eax, pd_table
    or eax, 0b11
    mov [pdp_table], eax

    ; Mapping 1 Go (Huge Pages 2Mo)
    mov ecx, 0
.map_pd:
    mov eax, 0x200000
    mul ecx
    or eax, 0x83
    mov [pd_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd

    ; Passage Long Mode
    mov eax, pml4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5 ; PAE
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8 ; LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31 ; Pagination
    mov cr0, eax

    lgdt [gdt64_ptr]
    jmp 0x08:long_mode_entry

bits 64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Initialisation propre de la pile 64-bit
    mov rsp, stack_top
    
    call kernel_main

.halt:
    hlt
    jmp .halt