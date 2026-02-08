ENTRY(_start)

PHDRS
{
    headers PT_PHDRS PHDRS ;
    text PT_LOAD FILEHDR PHDRS ;
    data PT_LOAD ;
}

SECTIONS
{
    . = 1M;

    .multiboot_header : {
        KEEP(*(.multiboot_header))
    } :text

    .text : ALIGN(4K) {
        *(.text)
    } :text

    .rodata : ALIGN(4K) {
        *(.rodata)
    } :text

    .data : ALIGN(4K) {
        *(.data)
    } :data

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
    } :data

    _kernel_end = .;
}