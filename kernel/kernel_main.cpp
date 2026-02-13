#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "keyboard.h"

// --- 1. BLOC DE LIAISON C (Pour ISR.CPP et IDT.CPP) ---
extern "C" {
    void init_pics();
    
    // Définition de handle_keyboard demandée par le linker
    void handle_keyboard() {
        Keyboard::handle_interrupt();
    }

    // Définition de panic demandée par isr.cpp
    // On ajoute 'const' pour matcher exactement la signature attendue
    void panic(const char* message) {
        (void)message; // Évite le warning unused parameter
        
        // On dessine un écran rouge de la mort
        uint32_t* fb = reinterpret_cast<uint32_t*>(0xFD000000);
        for (int i = 0; i < 1024 * 768; i++) {
            fb[i] = 0x00FF0000; 
        }
        
        while (1) { 
            __asm__ volatile("hlt"); 
        }
    }
}

// --- 2. NAMESPACE GRAPHICS ---
namespace Graphics {
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

// --- 3. POINT D'ENTRÉE DU NOYAU ---
extern "C" void kernel_main() {
    // Affiche l'interface immédiatement
    Graphics::clear_screen(0x003366FF); // Bleu
    Graphics::draw_rect(0, 0, 1024, 40, 0x00222222);     // Barre de titre
    Graphics::draw_rect(200, 150, 600, 400, 0x00CCCCCC); // Fenêtre grise

    // Initialise le matériel
    init_pics();
    IDT::initialize();
    Keyboard::initialize();

    // Boucle infinie
    while (1) {
        __asm__ volatile("hlt");
    }
}