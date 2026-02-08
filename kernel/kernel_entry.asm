bits 32

section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic number (Multiboot2)
    dd 0                         ; architecture 0 (i386)
    dd header_end - header_start ; length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum

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
align 16
stack_bottom:
    resb 16384
stack_top:

section .data
align 16
gdt64:                           ; GDT 64 bits
    dq 0                         ; Null descriptor
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Code segment (64-bit, Ring 0)
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)           ; Data segment
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
global _start
extern kernel_main

_start:
    cli                         ; Désactiver les interruptions
    mov esp, stack_top          ; Initialiser la pile 32 bits

    ; --- 1. SETUP PAGINATION (Mapping 1 Go) ---
    
    ; PML4[0] -> PDP
    mov eax, pdp_table
    or eax, 0b11                ; Present + Writable
    mov [pml4_table], eax

    ; PDP[0] -> PD
    mov eax, pd_table
    or eax, 0b11                ; Present + Writable
    mov [pdp_table], eax

    ; Boucle pour mapper 512 entrées de 2Mo dans la PD (soit 1 Go total)
    ; Cela évite que le CPU crash si le kernel dépasse les 2 premiers Mo
    mov ecx, 0
.map_pd_table:
    mov eax, 0x200000           ; 2 Mo
    mul ecx
    or eax, 0b10000011          ; Present + Writable + Huge Page (bit 7)
    mov [pd_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd_table

    ; --- 2. ACTIVER LES MODES ---

    ; Charger PML4 dans CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Activer PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Activer Long Mode dans EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Activer la pagination
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Charger la GDT 64 bits
    lgdt [gdt64_ptr]

    ; Far jump vers le mode 64 bits
    jmp 0x08:long_mode_init

bits 64
long_mode_init:
    ; Reset des registres de segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Appel du kernel C/C++
    call kernel_main

    ; Boucle de sécurité
.halt:
    hlt
    jmp .halt