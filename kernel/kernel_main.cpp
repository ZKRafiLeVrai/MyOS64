#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "keyboard.h"

// --- 1. LIAISON AVEC LES AUTRES FICHIERS ---
extern "C" {
    void init_pics();
    // On force panic en "C" pour que isr.cpp (le handler d'interruption) le trouve
    void panic(const char* message);
    void handle_keyboard() {
        Keyboard::handle_interrupt();
    }
}

// --- 2. NOUVEAU SYSTÈME GRAPHIQUE (UEFI) ---
namespace Graphics {
    // Adresse Framebuffer UEFI VirtualBox (1024x768)
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

// --- 3. DÉFINITION DE PANIC (Pour corriger l'erreur LD) ---
extern "C" void panic(const char* message) {
    (void)message;
    Graphics::clear_screen(0x00FF0000); // Écran rouge si crash
    while (1) { __asm__ volatile("hlt"); }
}

// --- 4. POINT D'ENTRÉE PRINCIPAL ---
extern "C" void kernel_main() {
    // A. Initialisation Vidéo UEFI
    Graphics::clear_screen(0x003366FF); // Fond Bleu MyOS
    Graphics::draw_rect(0, 0, 1024, 40, 0x00222222);     // Barre de titre
    Graphics::draw_rect(212, 184, 600, 400, 0x00CCCCCC); // Fenêtre centrale

    // B. Initialisation Système
    init_pics();
    IDT::initialize();
    Keyboard::initialize();

    // C. Boucle de vie (Ton ancien code de clavier peut être remis ici)
    while (1) {
        asm volatile("hlt");
    }
}