#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "keyboard.h"

// Déclarations externes pour le linker
extern "C" void init_pics();
extern "C" void handle_keyboard() {
    Keyboard::handle_interrupt();
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

// Fonction Panic requise par isr.cpp
extern "C" void panic(const char* message) {
    Graphics::clear_screen(0x00FF0000); // Écran rouge
    while (1) { asm volatile("hlt"); }
}

extern "C" void kernel_main() {
    // 1. Affichage de base UEFI
    Graphics::clear_screen(0x003366FF); // Fond bleu
    Graphics::draw_rect(200, 150, 600, 400, 0x00CCCCCC); // Fenêtre grise
    Graphics::draw_rect(0, 0, 1024, 40, 0x00222222);     // Barre de titre

    // 2. Initialisation du système (Keyboard/IDT)
    init_pics();
    IDT::initialize();
    Keyboard::initialize();

    // Boucle principale
    while (1) {
        asm volatile("hlt");
    }
}