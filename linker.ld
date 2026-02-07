ENTRY(_start)

SECTIONS
{
    . = 1M;

    .multiboot_header ALIGN(8) : AT(ADDR(.multiboot_header))
    {
        *(.multiboot_header)
    }

    .text ALIGN(4K) : AT(ADDR(.text))
    {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata))
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data))
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss))
    {
        *(COMMON)
        *(.bss)
    }
}