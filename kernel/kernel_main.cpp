#include <stdint.h>
#include <stddef.h>

namespace Graphics {
    // Adresse typique du Framebuffer en UEFI VirtualBox
    // En production, il faudrait la lire via les tags Multiboot2
    uint32_t* FRAMEBUFFER = reinterpret_cast<uint32_t*>(0xFD000000);
    const int WIDTH = 1024;
    const int HEIGHT = 768;

    void put_pixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            FRAMEBUFFER[y * WIDTH + x] = color;
        }
    }

    void clear_screen(uint32_t color) {
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            FRAMEBUFFER[i] = color;
        }
    }

    void draw_rect(int x, int y, int w, int h, uint32_t color) {
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                put_pixel(i, j, color);
            }
        }
    }
}

extern "C" void kernel_main() {
    // 1. On nettoie l'écran en Bleu MyOS (0x003366FF)
    Graphics::clear_screen(0x003366FF);

    // 2. On dessine une barre de titre (Gris foncé)
    Graphics::draw_rect(0, 0, 1024, 40, 0x00222222);

    // 3. On dessine une fenêtre centrale (Gris clair)
    Graphics::draw_rect(212, 184, 600, 400, 0x00CCCCCC);

    // 4. On dessine un bouton "OK" (Vert)
    Graphics::draw_rect(462, 450, 100, 40, 0x0000AA00);

    // Boucle infinie
    while (1) {
        __asm__ volatile("hlt");
    }
}