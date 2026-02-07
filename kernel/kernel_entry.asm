; Multiboot2 Header
section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic number (multiboot2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; end tag
align 8
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

; Section BSS pour la pile
section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB de pile
stack_top:

; Section de code
section .text
bits 32

global _start
extern kernel_main

_start:
    ; Configurer la pile
    mov esp, stack_top
    
    ; Sauvegarder les informations multiboot
    ; EBX contient l'adresse de la structure multiboot
    ; EAX contient le magic number
    
    ; Appeler le kernel principal
    call kernel_main
    
    ; Si le kernel retourne, boucle infinie
    cli
.hang:
    hlt
    jmp .hang
```

### 📝 Mettre à Jour grub.cfg

Assure-toi que `grub.cfg` utilise **multiboot2** :
```
set timeout=5
set default=0

menuentry "MyOS64" {
    multiboot2 /boot/myos.bin
    boot
}

menuentry "MyOS64 (Mode texte)" {
    multiboot2 /boot/myos.bin
    boot
}

menuentry "Redémarrer" {
    reboot
}

menuentry "Éteindre" {
    halt
}