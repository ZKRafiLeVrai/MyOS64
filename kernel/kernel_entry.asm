; --- kernel_entry.asm ---
bits 32

section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic number (Multiboot2)
    dd 0                         ; architecture 0 (i386)
    dd header_end - header_start ; length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum

    ; Tag obligatoire
    dw 0
    dw 0
    dd 8
header_end:

section .bss
align 4096
pml4_table:
    resb 4096
pdp_table:
    resb 4096
pd_table:
    resb 4096
align 16
stack_bottom:
    resb 16384
stack_top:

section .data
align 16
gdt64:                           ; GDT 64 bits minimale
    dq 0                         ; Null descriptor
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Code segment
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)           ; Data segment
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
global _start
extern kernel_main

_start:
    ; On bloque tout de suite les interruptions
    cli
    
    ; Initialisation de la pile
    mov esp, stack_top

    ; --- FIX RYZEN / VBOX : RAZ des registres de segment ---
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax

    ; --- 1. SETUP PAGINATION ---
    ; On lie PML4 -> PDP
    mov eax, pdp_table
    or eax, 0b11                 ; Present + Writable
    mov [pml4_table], eax

    ; On lie PDP -> PD
    mov eax, pd_table
    or eax, 0b11                 ; Present + Writable
    mov [pdp_table], eax

    ; On mappe la première page de 2Mo (Huge Page)
    ; 0x83 = Present + Writable + Page Size (bit 7)
    mov eax, 0x000083
    mov [pd_table], eax

    ; --- 2. PASSAGE LONG MODE ---
    ; Charger PML4 dans CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Activer PAE (Obligatoire pour le 64 bits)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Activer LME (Long Mode Enable) dans le MSR EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Activer la pagination pour déclencher le mode
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Charger la GDT 64 bits
    lgdt [gdt64_ptr]

    ; Saut vers le code 64 bits (le 0x08 correspond au .code de ta GDT)
    jmp 0x08:long_mode_entry

bits 64
long_mode_entry:
    ; Reset des registres de segment pour le 64 bits
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Enfin, on appelle ton kernel
    call kernel_main

    ; Si on revient ici, on boucle
.halt:
    hlt
    jmp .halt