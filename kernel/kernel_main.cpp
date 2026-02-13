#include <stdint.h>
#include <stddef.h>

// On force le linkage C pour que idt/isr puissent trouver les fonctions
extern "C" {
    void init_pics();
    void handle_keyboard();
    void panic(const char* message);
}

namespace VGA {
    static uint16_t* BUFFER = reinterpret_cast<uint16_t*>(0xB8000);
    constexpr size_t WIDTH = 80;
    constexpr size_t HEIGHT = 25;

    enum Color : uint8_t {
        BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4, MAGENTA = 5, BROWN = 6,
        LIGHT_GREY = 7, DARK_GREY = 8, LIGHT_BLUE = 9, LIGHT_GREEN = 10,
        LIGHT_CYAN = 11, LIGHT_RED = 12, LIGHT_MAGENTA = 13, YELLOW = 14, WHITE = 15
    };

    inline uint8_t make_color(Color fg, Color bg) { return fg | (bg << 4); }
    inline uint16_t make_entry(char c, uint8_t color) {
        return static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);
    }
}

class Terminal {
public:
    size_t row;
    size_t column;
    uint8_t color;

    Terminal() : row(0), column(0), color(VGA::make_color(VGA::WHITE, VGA::BLACK)) {}
    void set_color(VGA::Color fg, VGA::Color bg) { color = VGA::make_color(fg, bg); }

    void clear() {
        for (size_t y = 0; y < VGA::HEIGHT; y++) {
            for (size_t x = 0; x < VGA::WIDTH; x++) {
                VGA::BUFFER[y * VGA::WIDTH + x] = VGA::make_entry(' ', color);
            }
        }
        row = 0; column = 0;
    }

    void scroll() {
        for (size_t y = 0; y < VGA::HEIGHT - 1; y++) {
            for (size_t x = 0; x < VGA::WIDTH; x++) {
                VGA::BUFFER[y * VGA::WIDTH + x] = VGA::BUFFER[(y + 1) * VGA::WIDTH + x];
            }
        }
        for (size_t x = 0; x < VGA::WIDTH; x++) {
            VGA::BUFFER[(VGA::HEIGHT - 1) * VGA::WIDTH + x] = VGA::make_entry(' ', color);
        }
        row = VGA::HEIGHT - 1;
    }

    void putchar(char c) {
        if (c == '\n') {
            column = 0;
            if (++row == VGA::HEIGHT) scroll();
            return;
        }
        VGA::BUFFER[row * VGA::WIDTH + column] = VGA::make_entry(c, color);
        if (++column == VGA::WIDTH) {
            column = 0;
            if (++row == VGA::HEIGHT) scroll();
        }
    }

    void write(const char* str) { while (*str) putchar(*str++); }
    void write_line(const char* str) { write(str); putchar('\n'); }

    void write_at(size_t x, size_t y, const char* str, uint8_t custom_color) {
        size_t old_color = color; size_t old_row = row; size_t old_column = column;
        color = custom_color; row = y; column = x;
        while (*str && column < VGA::WIDTH) {
            VGA::BUFFER[row * VGA::WIDTH + column] = VGA::make_entry(*str++, color);
            column++;
        }
        color = old_color; row = old_row; column = old_column;
    }

    void draw_box(size_t x, size_t y, size_t width, size_t height, uint8_t box_color) {
        VGA::BUFFER[y * VGA::WIDTH + x] = VGA::make_entry('+', box_color);
        for (size_t i = 1; i < width - 1; i++) VGA::BUFFER[y * VGA::WIDTH + x + i] = VGA::make_entry('-', box_color);
        VGA::BUFFER[y * VGA::WIDTH + x + width - 1] = VGA::make_entry('+', box_color);
        for (size_t i = 1; i < height - 1; i++) {
            VGA::BUFFER[(y + i) * VGA::WIDTH + x] = VGA::make_entry('|', box_color);
            VGA::BUFFER[(y + i) * VGA::WIDTH + x + width - 1] = VGA::make_entry('|', box_color);
        }
        VGA::BUFFER[(y + height - 1) * VGA::WIDTH + x] = VGA::make_entry('+', box_color);
        for (size_t i = 1; i < width - 1; i++) VGA::BUFFER[(y + height - 1) * VGA::WIDTH + x + i] = VGA::make_entry('-', box_color);
        VGA::BUFFER[(y + height - 1) * VGA::WIDTH + x + width - 1] = VGA::make_entry('+', box_color);
    }
};

Terminal terminal;

#include "idt.h"
#include "keyboard.h"

// IMPLÉMENTATION DES FONCTIONS AVEC LINKAGE C
extern "C" void panic(const char* message) {
    terminal.set_color(VGA::WHITE, VGA::RED);
    terminal.clear();
    terminal.write("KERNEL PANIC: ");
    terminal.write_line(message);
    asm volatile("cli");
    while (1) { asm volatile("hlt"); }
}

extern "C" void handle_keyboard() {
    Keyboard::handle_interrupt();
}

void draw_gui() {
    terminal.clear();
    terminal.set_color(VGA::WHITE, VGA::BLUE);
    for (size_t i = 0; i < VGA::WIDTH; i++) terminal.putchar(' ');
    terminal.write_at(20, 0, "MyOS64 - 64-bit v0.1", VGA::make_color(VGA::YELLOW, VGA::BLUE));
    terminal.write_at(5, 19, "Command Prompt:", VGA::make_color(VGA::LIGHT_GREEN, VGA::BLACK));
    terminal.write_at(5, 20, "> ", VGA::make_color(VGA::YELLOW, VGA::BLACK));
}

static char command_buffer[256];
static size_t command_pos = 0;

extern "C" void kernel_main() {
    draw_gui();
    //init_pics();
    //IDT::initialize();
    //Keyboard::initialize();

    terminal.row = 20;
    terminal.column = 7;

    while (1) {
        if (Keyboard::has_key()) {
            char c = Keyboard::get_char();
            if (c == '\n') {
                terminal.write_line("");
                command_pos = 0;
                terminal.write_at(5, 20, "> ", VGA::make_color(VGA::YELLOW, VGA::BLACK));
            } else if (command_pos < 255) {
                command_buffer[command_pos++] = c;
                terminal.putchar(c);
            }
        }
        asm volatile("hlt");
    }
}