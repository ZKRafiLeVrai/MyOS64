#include <stdint.h>
#include <stddef.h>

// Dimensions VGA text mode
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

// Variables du terminal
static uint16_t* terminal_buffer = (uint16_t*) 0xB8000;
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

// Déclaration anticipée
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

// Créer une entrée VGA
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

// Créer une couleur VGA
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

// Calculer la longueur d'une chaîne
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

// Définir la couleur du terminal
void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

// Initialiser le terminal
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(7, 0); // Blanc sur noir
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

// Faire défiler l'écran
void terminal_scroll(void) {
    // Copier toutes les lignes vers le haut
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    
    // Effacer la dernière ligne
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    
    terminal_row = VGA_HEIGHT - 1;
}

// Écrire un caractère
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        return;
    }
    
    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    }
    
    if (c == '\t') {
        terminal_column = (terminal_column + 4) & ~3;
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_scroll();
            }
        }
        return;
    }
    
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
}

// Écrire un caractère à une position spécifique
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

// Écrire une chaîne
void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

// Écrire une chaîne terminée par null
void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

// Effacer l'écran
void terminal_clear(void) {
    terminal_initialize();
}

// Obtenir la position du curseur
void terminal_get_cursor(size_t* x, size_t* y) {
    *x = terminal_column;
    *y = terminal_row;
}

// Définir la position du curseur
void terminal_set_cursor(size_t x, size_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        terminal_column = x;
        terminal_row = y;
    }
}