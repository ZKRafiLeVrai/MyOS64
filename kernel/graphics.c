#include <stdint.h>
#include <stddef.h>

// Framebuffer VBE (VESA BIOS Extensions)
static uint32_t* framebuffer = (uint32_t*)0xFD000000;
static uint32_t screen_width = 1024;
static uint32_t screen_height = 768;
static uint32_t pitch = 1024 * 4;

// Initialiser le mode graphique
void graphics_init(void) {
    // Le mode graphique sera configuré par GRUB
    // via multiboot2 avec framebuffer
}

// Dessiner un pixel
void graphics_draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < (int)screen_width && y >= 0 && y < (int)screen_height) {
        framebuffer[y * screen_width + x] = color;
    }
}

// Effacer l'écran
void graphics_clear_screen(uint32_t color) {
    for (uint32_t y = 0; y < screen_height; y++) {
        for (uint32_t x = 0; x < screen_width; x++) {
            framebuffer[y * screen_width + x] = color;
        }
    }
}

// Dessiner un rectangle
void graphics_draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            graphics_draw_pixel(x + dx, y + dy, color);
        }
    }
}

// Dessiner un rectangle avec bordure
void graphics_draw_rect_border(int x, int y, int width, int height, 
                               uint32_t fill_color, uint32_t border_color, int border_width) {
    // Remplissage
    graphics_draw_rect(x, y, width, height, fill_color);
    
    // Bordures
    for (int i = 0; i < border_width; i++) {
        // Haut
        graphics_draw_rect(x + i, y + i, width - 2*i, 1, border_color);
        // Bas
        graphics_draw_rect(x + i, y + height - 1 - i, width - 2*i, 1, border_color);
        // Gauche
        graphics_draw_rect(x + i, y + i, 1, height - 2*i, border_color);
        // Droite
        graphics_draw_rect(x + width - 1 - i, y + i, 1, height - 2*i, border_color);
    }
}

// Dessiner une ligne horizontale
void graphics_draw_hline(int x, int y, int width, uint32_t color) {
    for (int i = 0; i < width; i++) {
        graphics_draw_pixel(x + i, y, color);
    }
}

// Dessiner une ligne verticale
void graphics_draw_vline(int x, int y, int height, uint32_t color) {
    for (int i = 0; i < height; i++) {
        graphics_draw_pixel(x, y + i, color);
    }
}

// Dessiner un cercle (algorithme de Bresenham)
void graphics_draw_circle(int cx, int cy, int radius, uint32_t color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (x <= y) {
        graphics_draw_pixel(cx + x, cy + y, color);
        graphics_draw_pixel(cx - x, cy + y, color);
        graphics_draw_pixel(cx + x, cy - y, color);
        graphics_draw_pixel(cx - x, cy - y, color);
        graphics_draw_pixel(cx + y, cy + x, color);
        graphics_draw_pixel(cx - y, cy + x, color);
        graphics_draw_pixel(cx + y, cy - x, color);
        graphics_draw_pixel(cx - y, cy - x, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Copier une région de pixels
void graphics_copy_region(int src_x, int src_y, int dst_x, int dst_y, 
                         int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t color = framebuffer[(src_y + y) * screen_width + (src_x + x)];
            graphics_draw_pixel(dst_x + x, dst_y + y, color);
        }
    }
}

// Obtenir les dimensions de l'écran
void graphics_get_screen_size(uint32_t* width, uint32_t* height) {
    *width = screen_width;
    *height = screen_height;
}
