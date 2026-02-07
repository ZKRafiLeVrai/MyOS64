section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic number
    dd 0                          ; architecture (0 = i386 protected mode)
    dd header_end - header_start  ; header length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum

    ; Information request tag - demande les infos de boot
align 8
info_request_tag_start:
    dw 1                          ; type = 1 (information request)
    dw 0                          ; flags
    dd info_request_tag_end - info_request_tag_start ; size
    dd 4                          ; request basic memory info
    dd 6                          ; request memory map
info_request_tag_end:

    ; Framebuffer tag - demande un framebuffer (optionnel mais recommandé)
align 8
framebuffer_tag_start:
    dw 5                          ; type = 5 (framebuffer)
    dw 0                          ; flags
    dd framebuffer_tag_end - framebuffer_tag_start ; size
    dd 1024                       ; width
    dd 768                        ; height
    dd 32                         ; depth
framebuffer_tag_end:

    ; End tag - obligatoire
align 8
    dw 0                          ; type = 0 (end tag)
    dw 0                          ; flags
    dd 8                          ; size
header_end:

section .bss
align 16
stack_bottom:
    resb 16384                    ; 16 KB stack
stack_top:

section .text
bits 32
global _start
extern kernel_main

_start:
    ; Désactiver les interruptions
    cli
    
    ; Configurer la pile
    mov esp, stack_top
    
    ; Initialiser les tables de pages
    call setup_page_tables
    
    ; Passer en mode long (64-bit)
    ; 1. Activer PAE (Physical Address Extension)
    mov eax, cr4
    or eax, (1 << 5)              ; bit 5 = PAE
    mov cr4, eax
    
    ; 2. Charger la table de pages (PML4)
    mov eax, pml4_table
    mov cr3, eax
    
    ; 3. Activer le mode long dans EFER
    mov ecx, 0xC0000080           ; EFER MSR
    rdmsr
    or eax, (1 << 8)              ; bit 8 = LME (Long Mode Enable)
    wrmsr
    
    ; 4. Activer le paging pour entrer en mode long
    mov eax, cr0
    or eax, (1 << 31)             ; bit 31 = PG (Paging)
    mov cr0, eax
    
    ; Maintenant on est en mode long (compatibility mode)
    ; Charger un GDT 64-bit et un saut far pour passer en vrai 64-bit
    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_start

bits 64
long_mode_start:
    ; Maintenant on est vraiment en 64-bit !
    
    ; Recharger les segments
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Reconfigurer la pile en 64-bit
    mov rsp, stack_top
    
    ; Appeler le kernel
    call kernel_main
    
    ; Si le kernel retourne, boucle infinie
.hang:
    cli
    hlt
    jmp .hang

; === GDT 64-bit ===
section .rodata
gdt64:
    dq 0                          ; Null descriptor
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Code segment (64-bit)
.data: equ $ - gdt64
    dq (1<<44) | (1<<47)          ; Data segment
.pointer:
    dw $ - gdt64 - 1              ; Limit
    dq gdt64                      ; Base

; === Tables de pages (identity mapping des premiers 2 MB) ===
section .bss
align 4096
pml4_table:
    resb 4096
pdp_table:
    resb 4096
pd_table:
    resb 4096

; === Initialisation des tables de pages ===
section .text
bits 32
setup_page_tables:
    ; PML4[0] -> PDP Table
    mov eax, pdp_table
    or eax, 0b11                  ; Present + Writable
    mov [pml4_table], eax
    
    ; PDP[0] -> PD Table
    mov eax, pd_table
    or eax, 0b11                  ; Present + Writable
    mov [pdp_table], eax
    
    ; PD[0] -> 2MB page (identity map 0-2MB)
    mov eax, 0
    or eax, 0b10000011            ; Present + Writable + Huge page
    mov [pd_table], eax
    
    ret
