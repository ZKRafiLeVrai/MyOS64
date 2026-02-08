section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic
    dd 0                         ; i386
    dd header_end - header_start ; length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; Tag de fin (obligatoire pour Multiboot2)
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
    resb 16384
stack_top:

section .data
align 16
gdt64:
    dq 0 ; null descriptor
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code descriptor (flat 64-bit)
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)           ; data descriptor
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
bits 32
global _start
extern kernel_main

_start:
    cli
    mov esp, stack_top

    ; 1. Vérification rapide (si EAX != magic, on s'arrête)
    cmp eax, 0x36d76289
    jne .error

    ; 2. Configuration des tables de pages
    ; On lie PML4 -> PDP
    mov eax, pdp_table
    or eax, 0b11 ; present + writable
    mov [pml4_table], eax

    ; On lie PDP -> PD
    mov eax, pd_table
    or eax, 0b11 ; present + writable
    mov [pdp_table], eax

    ; On mappe une page de 2Mo (Huge Page)
    ; 0x83 = Present + Writable + HugePage (bit 7)
    mov eax, 0x83
    mov [pd_table], eax

    ; 3. Activer PAE (Obligatoire pour le Long Mode)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 4. Charger la PML4 dans CR3
    mov eax, pml4_table
    mov cr3, eax

    ; 5. Activer le Long Mode dans le MSR EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 6. Activer le Paging pour entrer officiellement en mode compatibilité
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 7. Charger la GDT 64 bits et faire le saut définitif
    lgdt [gdt64_ptr]
    jmp 0x08:long_mode

.error:
    mov dword [0xb8000], 0x4f524f45 ; Affiche "ER" en rouge sur l'écran si erreur
    hlt

bits 64
long_mode:
    ; Charger les registres de segments avec le sélecteur de données (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    call kernel_main
    hlt