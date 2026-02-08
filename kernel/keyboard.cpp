#include "keyboard.h"
#include <stddef.h>  // Ajoute ça pour size_t
#include <stdint.h>  // Ajoute ça pour uint8_t, uint16_t
namespace Keyboard {
    // Keyboard buffer
    constexpr size_t BUFFER_SIZE = 256;
    static char buffer[BUFFER_SIZE];
    static size_t read_pos = 0;
    static size_t write_pos = 0;
    
    // Keyboard state
    static bool shift_pressed = false;
    static bool ctrl_pressed = false;
    static bool alt_pressed = false;
    static bool caps_lock = false;
    
    // Port I/O
    static inline void outb(uint16_t port, uint8_t value) {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
    }
    
    static inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }
    
    // US QWERTY keyboard layout
    static const char scancode_to_ascii[] = {
        0,   0,   '1', '2', '3', '4', '5', '6',     // 0x00-0x07
        '7', '8', '9', '0', '-', '=', '\b', '\t',   // 0x08-0x0F
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',     // 0x10-0x17
        'o', 'p', '[', ']', '\n', 0,   'a', 's',    // 0x18-0x1F
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     // 0x20-0x27
        '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',   // 0x28-0x2F
        'b', 'n', 'm', ',', '.', '/', 0,   '*',     // 0x30-0x37
        0,   ' ', 0,   0,   0,   0,   0,   0,       // 0x38-0x3F (ALT, SPACE, CAPS, F1-F5)
        0,   0,   0,   0,   0,   0,   0,   '7',     // 0x40-0x47 (F6-F10, NUM, SCROLL, KP7)
        '8', '9', '-', '4', '5', '6', '+', '1',     // 0x48-0x4F (KP...)
        '2', '3', '0', '.'                          // 0x50-0x53 (KP...)
    };
    
    static const char scancode_to_ascii_shift[] = {
        0,   0,   '!', '@', '#', '$', '%', '^',     // 0x00-0x07
        '&', '*', '(', ')', '_', '+', '\b', '\t',   // 0x08-0x0F
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',     // 0x10-0x17
        'O', 'P', '{', '}', '\n', 0,   'A', 'S',    // 0x18-0x1F
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     // 0x20-0x27
        '"', '~', 0,   '|', 'Z', 'X', 'C', 'V',     // 0x28-0x2F
        'B', 'N', 'M', '<', '>', '?', 0,   '*',     // 0x30-0x37
        0,   ' ', 0,   0,   0,   0,   0,   0,       // 0x38-0x3F
        0,   0,   0,   0,   0,   0,   0,   '7',     // 0x40-0x47
        '8', '9', '-', '4', '5', '6', '+', '1',     // 0x48-0x4F
        '2', '3', '0', '.'                          // 0x50-0x53
    };
    
    void initialize() {
        // Clear buffer
        read_pos = 0;
        write_pos = 0;
        
        // Enable keyboard
        while (inb(0x64) & 0x02);  // Wait for input buffer to be clear
        outb(0x64, 0xAE);          // Enable keyboard
        
        while (inb(0x64) & 0x02);
        outb(0x64, 0x20);          // Read command byte
        
        while (!(inb(0x64) & 0x01));
        uint8_t status = inb(0x60) | 1;  // Enable IRQ1
        
        while (inb(0x64) & 0x02);
        outb(0x64, 0x60);          // Write command byte
        
        while (inb(0x64) & 0x02);
        outb(0x60, status);
    }
    
    void handle_interrupt() {
        uint8_t scancode = inb(0x60);
        
        // Handle special keys (pressed)
        if (scancode == 0x2A || scancode == 0x36) {  // Shift
            shift_pressed = true;
            return;
        }
        if (scancode == 0x1D) {  // Ctrl
            ctrl_pressed = true;
            return;
        }
        if (scancode == 0x38) {  // Alt
            alt_pressed = true;
            return;
        }
        if (scancode == 0x3A) {  // Caps Lock
            caps_lock = !caps_lock;
            return;
        }
        
        // Handle special keys (released)
        if (scancode == 0xAA || scancode == 0xB6) {  // Shift released
            shift_pressed = false;
            return;
        }
        if (scancode == 0x9D) {  // Ctrl released
            ctrl_pressed = false;
            return;
        }
        if (scancode == 0xB8) {  // Alt released
            alt_pressed = false;
            return;
        }
        
        // Ignore key releases
        if (scancode & 0x80) {
            return;
        }
        
        // Convert scancode to ASCII
        char ascii = 0;
        if (scancode < sizeof(scancode_to_ascii)) {
            if (shift_pressed) {
                ascii = scancode_to_ascii_shift[scancode];
            } else {
                ascii = scancode_to_ascii[scancode];
                // Apply caps lock to letters
                if (caps_lock && ascii >= 'a' && ascii <= 'z') {
                    ascii -= 32;
                }
            }
        }
        
        // Add to buffer if valid
        if (ascii != 0) {
            size_t next_write = (write_pos + 1) % BUFFER_SIZE;
            if (next_write != read_pos) {  // Buffer not full
                buffer[write_pos] = ascii;
                write_pos = next_write;
            }
        }
    }
    
    bool has_key() {
        return read_pos != write_pos;
    }
    
    char get_char() {
        if (read_pos == write_pos) {
            return 0;  // No key available
        }
        
        char c = buffer[read_pos];
        read_pos = (read_pos + 1) % BUFFER_SIZE;
        return c;
    }
}
