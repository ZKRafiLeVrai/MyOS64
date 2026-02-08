#ifndef IDT_H
#define IDT_H

#include <stdint.h>

namespace IDT {
    // IDT entry structure
    struct Entry {
        uint16_t offset_low;    // Offset bits 0-15
        uint16_t selector;       // Code segment selector
        uint8_t ist;            // Interrupt Stack Table offset
        uint8_t type_attr;      // Type and attributes
        uint16_t offset_mid;    // Offset bits 16-31
        uint32_t offset_high;   // Offset bits 32-63
        uint32_t zero;          // Reserved
    } __attribute__((packed));
    
    // IDT pointer structure
    struct Pointer {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed));
    
    // Initialize IDT
    void initialize();
    
    // Set an IDT entry
    void set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags);
}

#endif
