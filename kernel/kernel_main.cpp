#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "keyboard.h"

// --- LIAISON C POUR LE LINKER (Répare l'erreur isr.cpp) ---
extern "C" {
    void init_pics();
    void handle_keyboard() { Keyboard::handle_interrupt(); }
    
    // Répare l'erreur "undefined reference to panic"
    void panic(const char* message) {
        // En cas de panic, on remplit l'écran de rouge (UEFI)
        uint32_t* fb = reinterpret_cast<uint32_t*>(0xFD000000);
        for (int i = 0; i < 1024 * 768; i++) fb[i] = 0x00FF0000;
        while (1) { __asm__ volatile("hlt"); }
    }
}

namespace Graphics {
    // Adresse Framebuffer UEFI VirtualBox
    uint32_t* FRAMEBUFFER = reinterpret_cast<uint32_t*>(0xFD000000);
    const int WIDTH = 1024;
    const int HEIGHT = 768;

    // Simulation du mode texte en UEFI (Dessin de pixels)
    void put_pixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            FRAMEBUFFER[y * WIDTH + x] = color;
    }

    // Dessine un bloc pour simuler un caractère (en attendant une vraie police)
    void draw_char_box(int col, int row, uint32_t color) {
        int x = col * 9;  // Largeur d'un caractère simulé
        int y = row * 16; // Hauteur d'un caractère simulé
        for(int j=0; j<14; j++)
            for(int i=0; i<7; i++)
                put_pixel(x+i, y+j, color);
    }
}

// Ta classe Terminal originale, adaptée pour écrire des pixels
class Terminal {
public:
    size_t row;
    size_t column;
    uint32_t current_color;

    Terminal() : row(0), column(0), current_color(0x00FFFFFF) {}

    void clear() {
        for (int i = 0; i < 1024 * 768; i++) Graphics::FRAMEBUFFER[i] = 0;
        row = 0; column = 0;
    }

    void putchar(char c) {
        if (c == '\n') { column = 0; row++; return; }
        // On dessine un petit rectangle pour chaque lettre
        Graphics::draw_char_box(column, row, current_color);
        if (++column > 80) { column = 0; row++; }
    }

    void write(const char* str) { while (*str) putchar(*str++); }
    
    void write_at(size_t x, size_t y, const char* str, uint32_t color) {
        column = x; row = y; current_color = color;
        write(str);
    }
};

Terminal terminal;

// Tes fonctions de GUI originales
void draw_gui() {
    terminal.clear();
    // On simule ton GUI avec des couleurs UEFI (Bleu: 0x000000FF, Jaune: 0x00FFFF00)
    terminal.write_at(20, 0, "MyOS64 - UEFI GRAPHIC MODE ACTIVE", 0x00FFFF00);
    
    // On dessine tes boites originales en pixels
    for(int i=0; i<1024; i++) Graphics::FRAMEBUFFER[40 * 1024 + i] = 0x00FFFFFF; // Ligne de séparation
}

// Ton point d'entrée original
extern "C" void kernel_main() {
    draw_gui();

    // Initialisation
    init_pics();
    IDT::initialize();
    Keyboard::initialize();

    terminal.write_at(2, 20, "> ", 0x0000FF00);

    while (1) {
        if (Keyboard::has_key()) {
            char c = Keyboard::get_char();
            terminal.putchar(c);
        }
        asm volatile("hlt");
    }
}