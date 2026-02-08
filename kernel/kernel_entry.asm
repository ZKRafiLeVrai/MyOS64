; --- CONSTANTES ---
VM_MEM_VIDEO    equ 0xb8000
MAGIC_MB2       equ 0xe85250d6
ARCH_I386       equ 0

section .multiboot_header
align 8
header_start:
    dd MAGIC_MB2
    dd ARCH_I386
    dd header_end - header_start
    dd 0x100000000 - (MAGIC_MB2 + ARCH_I386 + (header_end - header_start))

    ; Tag de fin obligatoire
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
stack_bottom:
    resb 16384 ; 16 KB de pile
stack_top:

section .data
align 16
gdt64:
    dq 0 ; Null Descriptor
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Code segment: Conforming, Readable, Present, 64-bit
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)           ; Data segment: Writable, Present
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
bits 32
global _start
extern kernel_main

_start:
    cli                         ; Désactive les interruptions
    mov esp, stack_top          ; Initialise la pile 32 bits

    ; --- 1. NETTOYAGE MÉMOIRE (Indispensable sur Ryzen) ---
    ; On remplit de zéros les 3 pages de tables (12 Ko)
    mov edi, pml4_table
    xor eax, eax
    mov ecx, 3072
    rep stosd

    ; --- 2. CONFIGURATION DES TABLES DE PAGES ---
    ; PML4[0] -> PDP
    mov eax, pdp_table
    or eax, 0b11                ; Present + Writable
    mov [pml4_table], eax

    ; PDP[0] -> PD
    mov eax, pd_table
    or eax, 0b11                ; Present + Writable
    mov [pdp_table], eax

    ; PD[0] -> 2MB Huge Page
    ; 0x83 = Present + Writable + PageSize (bit 7)
    mov eax, 0x000083
    mov [pd_table], eax

    ; --- 3. ACTIVATION DES RÉGISTRES DE CONTRÔLE ---
    ; Charger PML4 dans CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Activer PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Activer le Long Mode dans le MSR EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8              ; LME (Long Mode Enable)
    wrmsr

    ; Activer la Pagination (Active le mode 64 bits de compatibilité)
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; --- 4. PASSAGE DÉFINITIF EN 64 BITS ---
    lgdt [gdt64_ptr]            ; Charge la GDT 64 bits
    jmp 0x08:long_mode_start    ; Saut lointain pour vider le pipeline 32 bits

bits 64
long_mode_start:
    ; Nettoyage des registres de segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Appel du kernel C/C++
    call kernel_main

    ; Si le kernel quitte, on bloque le CPU
.halt:
    hlt
    jmp .halt