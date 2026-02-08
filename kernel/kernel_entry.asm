bits 32

section .multiboot_header
align 4096                       ; Alignement page-size pour UEFI
header_start:
    dd 0xe85250d6                ; Magic
    dd 0                         ; i386
    dd header_end - header_start 
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; Tag d'alignement requis pour certains chargeurs UEFI
    dw 6, 0
    dd 8

    ; Tag de fin
    dw 0, 0
    dd 8
header_end:

section .text
align 4096
global _start
extern kernel_main

_start:
    cli
    ; Initialisation pile 32 bits
    mov esp, stack_top

    ; --- RAZ TOTAL DES TABLES DE PAGES ---
    ; On nettoie 4 pages (PML4, PDP, PD, et une marge)
    mov edi, pml4_table
    xor eax, eax
    mov ecx, 4096               ; 4096 * 4 / 4
    rep stosd

    ; --- SETUP PAGINATION ---
    mov eax, pdp_table
    or eax, 0b11                ; Present + Writable
    mov [pml4_table], eax

    mov eax, pd_table
    or eax, 0b11                ; Present + Writable
    mov [pdp_table], eax

    ; Mapping 1 Go (Huge Pages 2Mo)
    ; Note: On utilise 0x83 (Present + Writable + Huge)
    mov ecx, 0
.map_pd:
    mov eax, 0x200000
    mul ecx
    or eax, 0x83
    mov [pd_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd

    ; --- PASSAGE LONG MODE ---
    mov eax, pml4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5              ; PAE
    mov cr4, eax

    mov ecx, 0xC0000080         ; EFER MSR
    rdmsr
    or eax, 1 << 8              ; LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31             ; Paging
    mov cr0, eax

    lgdt [gdt64_ptr]
    
    ; Saut far jump 64 bits
    jmp 0x08:long_mode_entry

bits 64
long_mode_entry:
    ; Reset des segments pour le mode 64 bits
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Alignement de la pile pour l'ABI x86_64
    mov rsp, stack_top
    
    call kernel_main

.halt:
    hlt
    jmp .halt

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
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Code
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)           ; Data
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64