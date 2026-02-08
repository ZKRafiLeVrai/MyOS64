ENTRY(_start)

SECTIONS
{
    /* On commence à 1Mo */
    . = 1M;

    /* Le header Multiboot DOIT être en premier */
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

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
    }

    _kernel_end = .;
}