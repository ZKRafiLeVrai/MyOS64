#include <stdint.h>

// VGA text mode buffer
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(void) {
    // Effacer l'écran
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = 0x0F00 | ' '; // Blanc sur noir
    }
    
    // Message de test
    const char* message = "MyOS64 - Kernel Started!";
    int pos = 0;
    
    while (message[pos] != '\0') {
        vga_buffer[pos] = 0x0F00 | message[pos];
        pos++;
    }
    
    // Boucle infinie
    while(1) {
        __asm__ volatile ("hlt");
    }
}