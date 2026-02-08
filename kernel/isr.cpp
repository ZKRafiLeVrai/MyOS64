#include <stdint.h>

// Register state structure
struct Registers {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
};

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

// Port I/O functions
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Exception names
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Forward declaration of terminal write function
extern void panic(const char* message);

// ISR handler
extern "C" void isr_handler(Registers* regs) {
    if (regs->int_no < 32) {
        // CPU exception
        panic(exception_messages[regs->int_no]);
    }
}

// IRQ handler
extern "C" void irq_handler(Registers* regs) {
    // Send EOI to PICs
    if (regs->int_no >= 40) {
        outb(PIC2_COMMAND, PIC_EOI);  // Send EOI to slave PIC
    }
    outb(PIC1_COMMAND, PIC_EOI);      // Send EOI to master PIC
    
    // Handle specific IRQs
    switch (regs->int_no) {
        case 32:  // Timer IRQ
            // Timer handler (can be used for scheduling later)
            break;
        case 33:  // Keyboard IRQ
            handle_keyboard();
            break;
    }
}

// Keyboard handler (will be implemented properly later)
void handle_keyboard() {
    uint8_t scancode = inb(0x60);
    // For now, just read and discard
    // Proper keyboard handler will be added later
}

// Initialize PICs
void init_pics() {
    // ICW1 - begin initialization
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    // ICW2 - remap offset address of IDT
    outb(PIC1_DATA, 0x20);  // Master PIC vector offset
    outb(PIC2_DATA, 0x28);  // Slave PIC vector offset
    
    // ICW3 - setup cascading
    outb(PIC1_DATA, 0x04);  // Tell Master PIC there is a slave PIC at IRQ2
    outb(PIC2_DATA, 0x02);  // Tell Slave PIC its cascade identity
    
    // ICW4 - environment info
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Mask all interrupts except keyboard (IRQ1)
    outb(PIC1_DATA, 0xFD);  // Enable IRQ1 (keyboard)
    outb(PIC2_DATA, 0xFF);  // Disable all slave IRQs
}
