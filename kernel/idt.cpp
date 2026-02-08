#include "idt.h"

namespace IDT {
    // IDT entries (256 interrupts)
    static Entry idt_entries[256];
    static Pointer idt_ptr;
    
    // Declare ISR handlers (defined in idt_asm.asm)
    extern "C" {
        void isr0();
        void isr1();
        void isr2();
        void isr3();
        void isr4();
        void isr5();
        void isr6();
        void isr7();
        void isr8();
        void isr9();
        void isr10();
        void isr11();
        void isr12();
        void isr13();
        void isr14();
        void isr15();
        void isr16();
        void isr17();
        void isr18();
        void isr19();
        void isr20();
        void isr21();
        void isr22();
        void isr23();
        void isr24();
        void isr25();
        void isr26();
        void isr27();
        void isr28();
        void isr29();
        void isr30();
        void isr31();
        
        // IRQs (32-47)
        void irq0();
        void irq1();
        void irq2();
        void irq3();
        void irq4();
        void irq5();
        void irq6();
        void irq7();
        void irq8();
        void irq9();
        void irq10();
        void irq11();
        void irq12();
        void irq13();
        void irq14();
        void irq15();
    }
    
    void set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags) {
        idt_entries[num].offset_low = handler & 0xFFFF;
        idt_entries[num].offset_mid = (handler >> 16) & 0xFFFF;
        idt_entries[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
        idt_entries[num].selector = selector;
        idt_entries[num].ist = 0;
        idt_entries[num].type_attr = flags;
        idt_entries[num].zero = 0;
    }
    
    void initialize() {
        idt_ptr.limit = sizeof(idt_entries) - 1;
        idt_ptr.base = reinterpret_cast<uint64_t>(&idt_entries);
        
        // Clear IDT
        for (int i = 0; i < 256; i++) {
            set_gate(i, 0, 0, 0);
        }
        
        // Set up CPU exception handlers (0-31)
        set_gate(0, reinterpret_cast<uint64_t>(isr0), 0x08, 0x8E);
        set_gate(1, reinterpret_cast<uint64_t>(isr1), 0x08, 0x8E);
        set_gate(2, reinterpret_cast<uint64_t>(isr2), 0x08, 0x8E);
        set_gate(3, reinterpret_cast<uint64_t>(isr3), 0x08, 0x8E);
        set_gate(4, reinterpret_cast<uint64_t>(isr4), 0x08, 0x8E);
        set_gate(5, reinterpret_cast<uint64_t>(isr5), 0x08, 0x8E);
        set_gate(6, reinterpret_cast<uint64_t>(isr6), 0x08, 0x8E);
        set_gate(7, reinterpret_cast<uint64_t>(isr7), 0x08, 0x8E);
        set_gate(8, reinterpret_cast<uint64_t>(isr8), 0x08, 0x8E);
        set_gate(9, reinterpret_cast<uint64_t>(isr9), 0x08, 0x8E);
        set_gate(10, reinterpret_cast<uint64_t>(isr10), 0x08, 0x8E);
        set_gate(11, reinterpret_cast<uint64_t>(isr11), 0x08, 0x8E);
        set_gate(12, reinterpret_cast<uint64_t>(isr12), 0x08, 0x8E);
        set_gate(13, reinterpret_cast<uint64_t>(isr13), 0x08, 0x8E);
        set_gate(14, reinterpret_cast<uint64_t>(isr14), 0x08, 0x8E);
        set_gate(15, reinterpret_cast<uint64_t>(isr15), 0x08, 0x8E);
        set_gate(16, reinterpret_cast<uint64_t>(isr16), 0x08, 0x8E);
        set_gate(17, reinterpret_cast<uint64_t>(isr17), 0x08, 0x8E);
        set_gate(18, reinterpret_cast<uint64_t>(isr18), 0x08, 0x8E);
        set_gate(19, reinterpret_cast<uint64_t>(isr19), 0x08, 0x8E);
        set_gate(20, reinterpret_cast<uint64_t>(isr20), 0x08, 0x8E);
        set_gate(21, reinterpret_cast<uint64_t>(isr21), 0x08, 0x8E);
        set_gate(22, reinterpret_cast<uint64_t>(isr22), 0x08, 0x8E);
        set_gate(23, reinterpret_cast<uint64_t>(isr23), 0x08, 0x8E);
        set_gate(24, reinterpret_cast<uint64_t>(isr24), 0x08, 0x8E);
        set_gate(25, reinterpret_cast<uint64_t>(isr25), 0x08, 0x8E);
        set_gate(26, reinterpret_cast<uint64_t>(isr26), 0x08, 0x8E);
        set_gate(27, reinterpret_cast<uint64_t>(isr27), 0x08, 0x8E);
        set_gate(28, reinterpret_cast<uint64_t>(isr28), 0x08, 0x8E);
        set_gate(29, reinterpret_cast<uint64_t>(isr29), 0x08, 0x8E);
        set_gate(30, reinterpret_cast<uint64_t>(isr30), 0x08, 0x8E);
        set_gate(31, reinterpret_cast<uint64_t>(isr31), 0x08, 0x8E);
        
        // Set up IRQ handlers (32-47)
        set_gate(32, reinterpret_cast<uint64_t>(irq0), 0x08, 0x8E);
        set_gate(33, reinterpret_cast<uint64_t>(irq1), 0x08, 0x8E);
        set_gate(34, reinterpret_cast<uint64_t>(irq2), 0x08, 0x8E);
        set_gate(35, reinterpret_cast<uint64_t>(irq3), 0x08, 0x8E);
        set_gate(36, reinterpret_cast<uint64_t>(irq4), 0x08, 0x8E);
        set_gate(37, reinterpret_cast<uint64_t>(irq5), 0x08, 0x8E);
        set_gate(38, reinterpret_cast<uint64_t>(irq6), 0x08, 0x8E);
        set_gate(39, reinterpret_cast<uint64_t>(irq7), 0x08, 0x8E);
        set_gate(40, reinterpret_cast<uint64_t>(irq8), 0x08, 0x8E);
        set_gate(41, reinterpret_cast<uint64_t>(irq9), 0x08, 0x8E);
        set_gate(42, reinterpret_cast<uint64_t>(irq10), 0x08, 0x8E);
        set_gate(43, reinterpret_cast<uint64_t>(irq11), 0x08, 0x8E);
        set_gate(44, reinterpret_cast<uint64_t>(irq12), 0x08, 0x8E);
        set_gate(45, reinterpret_cast<uint64_t>(irq13), 0x08, 0x8E);
        set_gate(46, reinterpret_cast<uint64_t>(irq14), 0x08, 0x8E);
        set_gate(47, reinterpret_cast<uint64_t>(irq15), 0x08, 0x8E);
        
        // Load IDT
        asm volatile("lidt %0" : : "m"(idt_ptr));
        
        // Enable interrupts
        asm volatile("sti");
    }
}
