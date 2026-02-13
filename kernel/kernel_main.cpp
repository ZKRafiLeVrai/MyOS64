#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "keyboard.h"

// --- 1. Déclarations C pour le Linker ---
// On met tout dans un bloc extern "C" pour être certain que les noms sont propres
extern "C" {
    void init_pics();
    void panic(const char* message);
    void handle_keyboard();
}

namespace Graphics {
    // Adresse Framebuffer UEFI VirtualBox
    uint32_t* FRAMEBUFFER = reinterpret_cast<uint32_t*>(0xFD000000);
    const int WIDTH = 1024;
    const int HEIGHT = 768;

    void clear_screen(uint32_t color) {
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            FRAMEBUFFER[i] = color;
        }
    }

    void draw_rect(int x, int y, int w, int h, uint32_t color) {
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                if (i >= 0 && i < WIDTH && j >= 0 && j < HEIGHT) {
                    FRAMEBUFFER[j * WIDTH + i] = color;
                }
            }
        }
    }
}

// --- 2. Définitions des fonctions C ---

extern "C" void handle_keyboard() {
    Keyboard::handle_interrupt();
}

extern "C" void panic(const char* message) {
    (void)message; // Empêche le warning unused parameter
    Graphics::clear_screen(0x00FF0000); // Écran rouge "BSOD"
    while (1) { 
        __asm__ volatile("hlt"); 
    }
}

// --- 3. Point d'entrée ---

extern "C" void kernel_main() {
    // 1. Affichage immédiat (pour confirmer que le noyau respire)
    Graphics::clear_screen(0x003366FF); // Fond bleu
    Graphics::draw_rect(200, 150, 600, 400, 0x00CCCCCC); // Fenêtre grise
    Graphics::draw_rect(0, 0, 1024, 40, 0x00222222);     // Barre de titre

    // 2. Initialisation du matériel
    init_pics();
    IDT::initialize();
    Keyboard::initialize();

    // 3. Boucle d'attente
    while (1) {
        __asm__ volatile("hlt");
    }
}