bits 32

section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; Magic number Multiboot2
    dd 0                         ; Architecture i386
    dd header_end - header_start ; Longueur du header
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; Checksum

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
gdt64:                           ; Global Descriptor Table 64 bits
    dq 0                         ; Segment nul
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Segment Code (64-bit, Ring 0)
.data: equ $ - gdt64
    dq (1<<41) | (1<<44) | (1<<47)           ; Segment Données
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .text
global _start
extern kernel_main

_start:
    cli                         ; Désactive les interruptions
    mov esp, stack_top          ; Initialise la pile 32 bits temporaire

    ; --- 1. NETTOYAGE DES TABLES DE PAGES (Vital pour Ryzen) ---
    ; On remplit les tables PML4, PDP et PD de zéros pour éviter les "bit résidus"
    mov edi, pml4_table
    xor eax, eax
    mov ecx, 3072               ; (4096 octets * 3 tables) / 4
    rep stosd

    ; --- 2. CONFIGURATION DE LA PAGINATION ---
    ; PML4 -> PDP
    mov eax, pdp_table
    or eax, 0b11                ; Present + Writable
    mov [pml4_table], eax

    ; PDP -> PD
    mov eax, pd_table
    or eax, 0b11                ; Present + Writable
    mov [pdp_table], eax

    ; Mapping de 1 Go via Huge Pages (2 Mo par page)
    ; Cela couvre largement ton kernel et ta pile
    mov ecx, 0
.map_pd_table:
    mov eax, 0x200000           ; 2 Mo
    mul ecx
    or eax, 0b10000011          ; Present + Writable + Huge Page (bit 7)
    mov [pd_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512                ; On remplit les 512 entrées
    jne .map_pd_table

    ; --- 3. PASSAGE EN LONG MODE ---
    ; Charger PML4 dans CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Activer PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Activer LME (Long Mode Enable) dans le MSR EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Activer la pagination (Déclenche le mode Long)
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Charger la GDT 64 bits
    lgdt [gdt64_ptr]

    ; Far jump pour passer réellement en mode 64 bits (CS = 0x08)
    jmp 0x08:long_mode_init

bits 64
long_mode_init:
    ; Reset des registres de segment pour le 64-bit
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; --- ALIGNEMENT DE LA PILE (ABI x86-64) ---
    ; La pile doit être alignée sur 16 octets avant un 'call'
    mov rsp, stack_top
    
    ; Appel du kernel écrit en C++
    call kernel_main

    ; Boucle de sécurité si kernel_main retourne
.halt:
    hlt
    jmp .halt