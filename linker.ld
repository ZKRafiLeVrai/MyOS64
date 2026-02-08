ENTRY(_start)

SECTIONS
{
    /* On commence à 2Mo pour laisser de la place aux headers ELF en mode UEFI */
    . = 2M;

    .multiboot_header : ALIGN(4K)
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
}