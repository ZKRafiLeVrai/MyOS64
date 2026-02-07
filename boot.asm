; Boot sector pour mode long (64-bit)
[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Afficher message de démarrage
    mov si, boot_msg
    call print_string
    
    ; Vérifier support 64-bit
    call check_long_mode
    
    ; Charger le kernel
    call load_kernel
    
    ; Activer A20
    call enable_a20
    
    ; Passer en mode protégé puis long mode
    call enter_protected_mode
    
    jmp $

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

check_long_mode:
    ; Vérifier CPUID
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz .no_long_mode
    
    ; Vérifier extended functions
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    
    ; Vérifier LM bit
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
    
.no_long_mode:
    mov si, no_64bit_msg
    call print_string
    jmp $

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

load_kernel:
    ; Utiliser BIOS INT 13h pour charger le kernel
    mov ah, 0x02        ; Fonction lecture
    mov al, 32          ; Nombre de secteurs
    mov ch, 0           ; Cylindre 0
    mov cl, 2           ; Secteur 2 (après boot sector)
    mov dh, 0           ; Tête 0
    mov bx, 0x1000      ; Adresse de chargement
    int 0x13
    jc .load_error
    ret
    
.load_error:
    mov si, load_error_msg
    call print_string
    jmp $

enter_protected_mode:
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp CODE_SEG:protected_mode_start

[BITS 32]
protected_mode_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Configurer pagination pour mode long
    call setup_paging
    
    ; Activer PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; Charger PML4
    mov eax, pml4_table
    mov cr3, eax
    
    ; Activer long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ; Activer paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    lgdt [gdt64_descriptor]
    jmp CODE64_SEG:long_mode_start

setup_paging:
    ; Initialiser tables de pagination
    mov edi, pml4_table
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd
    mov edi, cr3
    
    mov DWORD [edi], pdp_table | 3
    mov edi, pdp_table
    mov DWORD [edi], pd_table | 3
    mov edi, pd_table
    
    mov ebx, 0x00000003
    mov ecx, 512
.loop:
    mov DWORD [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .loop
    
    ret

[BITS 64]
long_mode_start:
    mov rax, DATA64_SEG
    mov ds, rax
    mov es, rax
    mov fs, rax
    mov gs, rax
    mov ss, rax
    
    ; Sauter au kernel
    mov rax, 0x100000
    jmp rax
    
    hlt

; GDT pour mode protégé
gdt_start:
gdt_null:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; GDT pour mode long
gdt64_start:
    dq 0x0
gdt64_code:
    dq 0x00209A0000000000
gdt64_data:
    dq 0x0000920000000000
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dq gdt64_start

CODE64_SEG equ gdt64_code - gdt64_start
DATA64_SEG equ gdt64_data - gdt64_start

; Messages
boot_msg db 'MyOS64 Booting...', 13, 10, 0
no_64bit_msg db 'ERROR: 64-bit not supported!', 13, 10, 0
load_error_msg db 'ERROR: Kernel load failed!', 13, 10, 0

; Tables de pagination
align 4096
pml4_table:
    times 512 dq 0
pdp_table:
    times 512 dq 0
pd_table:
    times 512 dq 0

times 510-($-$$) db 0
dw 0xAA55
