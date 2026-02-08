section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic number (multiboot 2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; tag de fin obligatoire
    dw 0
    dw 0
    dd 8
header_end:

section .bss
align 4096 ; Les tables de pages DOIVENT être alignées sur 4KB
pml4_table:
    resb 4096
pdp_table:
    resb 4096
pd_table:
    resb 4096
stack_bottom:
    resb 16384
stack_top:

section .data
align 16
gdt64:
    dq 0 ; entry 0 (null)
    dq 0x00209A0000000000  ; entry 8 (code segment)
    dq 0x0000920000000000  ; entry 16 (data segment)
gdt64_pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .text
bits 32
global _start
extern kernel_main

_start:
    cli
    mov esp, stack_top

    ; 1. Pointer la dernière entrée de la PDP vers la PD pour mapper 1Go
    ; On sature de zéros pour être sûr
    mov edi, pml4_table
    mov ecx, 3072 ; 4096 * 3 / 4
    xor eax, eax
    rep stosd

    ; 2. Setup Paging (Identity Map les premiers 2Mo)
    mov eax, pdp_table
    or eax, 0b11 ; present + writable
    mov [pml4_table], eax

    mov eax, pd_table
    or eax, 0b11 ; present + writable
    mov [pdp_table], eax

    mov eax, 0b10000011 ; present + writable + huge (2MB page)
    mov [pd_table], eax

    ; 3. Activer PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 4. Charger PML4
    mov eax, pml4_table
    mov cr3, eax

    ; 5. Activer Long Mode dans EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 6. Activer Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 7. Charger GDT et saut final
    lgdt [gdt64_pointer]
    jmp 0x08:long_mode

bits 64
long_mode:
    ; Nettoyage des registres segments
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Appel du noyau C++
    call kernel_main

.hang:
    hlt
    jmp .hang