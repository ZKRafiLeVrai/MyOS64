ENTRY(_start)

SECTIONS
{
    /* On commence à 2 Mo pour éviter les conflits UEFI */
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