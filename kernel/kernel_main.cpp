#include <stdint.h>
#include <stddef.h>

extern "C" {
    void init_pics();
    void handle_keyboard();
    void panic(const char* message);
}

// Pour l'UEFI, on a besoin des infos du Framebuffer (souvent passées par le bootloader)
struct Framebuffer {
    uint32_t* base_address;
    size_t width;
    size_t height;
    size_t pixels_per_scanline;
};

// On simule une police 8x8 très basique pour que tu puisses voir du texte
// Dans un vrai OS UEFI, on charge un fichier .psf
void draw_pixel(Framebuffer* fb, uint32_t x, uint32_t y, uint32_t color) {
    fb->base_address[y * fb->pixels_per_scanline + x] = color;
}

namespace VGA {
    // En UEFI, les couleurs sont en 32 bits (0xRRGGBB)
    enum Color : uint32_t {
        BLACK = 0x000000,
        BLUE = 0x0000AA,
        GREEN = 0x00AA00,
        CYAN = 0x00AAAA,
        RED = 0xAA0000,
        MAGENTA = 0xAA00AA,
        BROWN = 0xAA5500,
        LIGHT_GREY = 0xAAAAAA,
        DARK_GREY = 0x555555,
        LIGHT_BLUE = 0x5555FF,
        LIGHT_GREEN = 0x55FF55,
        LIGHT_CYAN = 0x55FFFF,
        LIGHT_RED = 0xFF5555,
        LIGHT_MAGENTA = 0xFF55FF,
        YELLOW = 0xFFFF55,
        WHITE = 0xFFFFFF
    };
}

class Terminal {
public:
    size_t row, column;
    uint32_t fg_color, bg_color;
    Framebuffer* fb;

    // On initialise avec une adresse par défaut (souvent 0xFD000000 ou passée par Limine/Gnu-EFI)
    Terminal() : row(0), column(0), fg_color(VGA::WHITE), bg_color(VGA::BLACK) {}

    void set_color(VGA::Color fg, VGA::Color bg) {
        fg_color = fg;
        bg_color = bg;
    }

    void clear() {
        for (size_t y = 0; y < fb->height; y++) {
            for (size_t x = 0; x < fb->width; x++) {
                draw_pixel(fb, x, y, bg_color);
            }
        }
        row = 0; column = 0;
    }

    // Dessine un carré 8x8 pour simuler un caractère (plus simple pour l'instant)
    void putchar(char c) {
        if (c == '\n') {
            column = 0;
            row++;
            return;
        }

        uint32_t start_x = column * 8;
        uint32_t start_y = row * 16;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 12; j++) {
                // On dessine un bloc pour chaque lettre
                // Note : Pour de vraies lettres, il faudra inclure une matrice de police
                draw_pixel(fb, start_x + i, start_y + j, fg_color);
            }
        }

        if (++column * 8 >= fb->width) {
            column = 0;
            row++;
        }
    }

    void write(const char* str) { while (*str) putchar(*str++); }
    void write_line(const char* str) { write(str); putchar('\n'); }
    
    void write_at(size_t x, size_t y, const char* str, uint32_t color_override) {
        uint32_t old_fg = fg_color;
        fg_color = color_override;
        column = x; row = y;
        write(str);
        fg_color = old_fg;
    }

    void draw_box(size_t x, size_t y, size_t w, size_t h, uint32_t color) {
        // Version simplifiée pour le mode graphique
        for(size_t i=0; i<w*8; i++) {
            draw_pixel(fb, x*8 + i, y*16, color);
            draw_pixel(fb, x*8 + i, (y+h)*16, color);
        }
    }
};

Terminal terminal;

#include "idt.h"
#include "keyboard.h"

extern "C" void panic(const char* message) {
    terminal.set_color(VGA::WHITE, VGA::RED);
    terminal.clear();
    terminal.write("KERNEL PANIC: ");
    terminal.write_line(message);
    asm volatile("cli; hlt");
}

// Le bootloader UEFI doit passer l'adresse du Framebuffer ici
extern "C" void kernel_main(Framebuffer* fb_ptr) {
    terminal.fb = fb_ptr; // On lie le terminal au framebuffer UEFI
    
    // Ton code de GUI reste identique
    terminal.clear();
    terminal.write_at(10, 0, "MyOS64 UEFI Mode", VGA::YELLOW);

    // Initialisation
    // init_pics();
    // IDT::initialize();
    // Keyboard::initialize();

    terminal.row = 20;
    terminal.column = 2;

    while (1) {
        if (Keyboard::has_key()) {
            char c = Keyboard::get_char();
            if (c == '\n') terminal.write_line("ENTER");
            else terminal.putchar(c);
        }
        asm volatile("hlt");
    }
}