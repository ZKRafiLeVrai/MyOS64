ENTRY(_start)

SECTIONS
{
    /* On commence à 1Mo, l'endroit standard pour les noyaux x86 */
    . = 1M;

    /* Header Multiboot en premier, aligné sur 8 octets */
    .multiboot_header : ALIGN(8)
    {
        KEEP(*(.multiboot_header))
    }

    /* Section Code : alignée sur 4Ko pour la pagination */
    .text : ALIGN(4K)
    {
        *(.text)
    }

    /* Données en lecture seule */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Données initialisées */
    .data : ALIGN(4K)
    {
        *(.data)
    }

    /* Section BSS : TRÈS IMPORTANT pour les tables de pages */
    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        /* On force l'alignement de fin pour éviter que 
           le CPU ne lise au-delà de la section */
        . = ALIGN(4K);
    }

    /* Symbole pour marquer la fin du noyau */
    _kernel_end = .;
}