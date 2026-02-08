#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

namespace Keyboard {
    // Initialize keyboard driver
    void initialize();
    
    // Handle keyboard interrupt
    void handle_interrupt();
    
    // Get last key pressed (returns 0 if no key available)
    char get_char();
    
    // Check if a key is available
    bool has_key();
}

#endif
