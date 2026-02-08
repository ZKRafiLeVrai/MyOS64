ENTRY(_start)

SECTIONS
{
    /* On commence à 1Mo */
    . = 1M;

    /* Le header Multiboot2 DOIT être en premier pour GRUB */
    .multiboot_header :
    {
        KEEP(*(.multiboot_header))
    }

    .text : ALIGN(4K)
    {
        *(.text)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        *(.data)
    }

    /* Section BSS alignée pour les tables de pages */
    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
    }

    _kernel_end = .;
}