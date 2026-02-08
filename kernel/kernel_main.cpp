#include <stdint.h>

#include <stddef.h>
extern "C" void kernel_main() {
    // Affiche un 'X' blanc sur fond bleu en haut à gauche
    unsigned short* vga = (unsigned short*)0xB8000;
    vga[0] = 0x1F58; 

    while(1) { __asm__("hlt"); }
}
extern "C" {
    void kernel_main();
    void init_pics();
    void handle_keyboard();
}
namespace VGA {

    static uint16_t* BUFFER = reinterpret_cast<uint16_t*>(0xB8000);

    constexpr size_t WIDTH = 80;

    constexpr size_t HEIGHT = 25;

   

    enum Color : uint8_t {

        BLACK = 0,

        BLUE = 1,

        GREEN = 2,

        CYAN = 3,

        RED = 4,

        MAGENTA = 5,

        BROWN = 6,

        LIGHT_GREY = 7,

        DARK_GREY = 8,

        LIGHT_BLUE = 9,

        LIGHT_GREEN = 10,

        LIGHT_CYAN = 11,

        LIGHT_RED = 12,

        LIGHT_MAGENTA = 13,

        YELLOW = 14,

        WHITE = 15

    };

   

    inline uint8_t make_color(Color fg, Color bg) {

        return fg | (bg << 4);

    }

   

    inline uint16_t make_entry(char c, uint8_t color) {

        return static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);

    }

}



// Terminal class for text display

class Terminal {

public:

    size_t row;

    size_t column;

    uint8_t color;

   


    Terminal() : row(0), column(0), color(VGA::make_color(VGA::WHITE, VGA::BLACK)) {}

   

    void set_color(VGA::Color fg, VGA::Color bg) {

        color = VGA::make_color(fg, bg);

    }

   

    void clear() {

        for (size_t y = 0; y < VGA::HEIGHT; y++) {

            for (size_t x = 0; x < VGA::WIDTH; x++) {

                const size_t index = y * VGA::WIDTH + x;

                VGA::BUFFER[index] = VGA::make_entry(' ', color);

            }

        }

        row = 0;

        column = 0;

    }

   

    void scroll() {

        // Move all lines up by one

        for (size_t y = 0; y < VGA::HEIGHT - 1; y++) {

            for (size_t x = 0; x < VGA::WIDTH; x++) {

                VGA::BUFFER[y * VGA::WIDTH + x] = VGA::BUFFER[(y + 1) * VGA::WIDTH + x];

            }

        }

       

        // Clear the last line

        for (size_t x = 0; x < VGA::WIDTH; x++) {

            VGA::BUFFER[(VGA::HEIGHT - 1) * VGA::WIDTH + x] = VGA::make_entry(' ', color);

        }

       

        row = VGA::HEIGHT - 1;

    }

   

    void putchar(char c) {

        if (c == '\n') {

            column = 0;

            if (++row == VGA::HEIGHT) {

                scroll();

            }

            return;

        }

       

        if (c == '\t') {

            column = (column + 4) & ~3;

            if (column >= VGA::WIDTH) {

                column = 0;

                if (++row == VGA::HEIGHT) {

                    scroll();

                }

            }

            return;

        }

       

        const size_t index = row * VGA::WIDTH + column;

        VGA::BUFFER[index] = VGA::make_entry(c, color);

       

        if (++column == VGA::WIDTH) {

            column = 0;

            if (++row == VGA::HEIGHT) {

                scroll();

            }

        }

    }

   

    void write(const char* str) {

        while (*str) {

            putchar(*str++);

        }

    }

   

    void write_line(const char* str) {

        write(str);

        putchar('\n');

    }

   

    void write_at(size_t x, size_t y, const char* str, uint8_t custom_color) {

        size_t old_color = color;

        size_t old_row = row;

        size_t old_column = column;

       

        color = custom_color;

        row = y;

        column = x;

       

        while (*str && column < VGA::WIDTH) {

            const size_t index = row * VGA::WIDTH + column;

            VGA::BUFFER[index] = VGA::make_entry(*str++, color);

            column++;

        }

       

        color = old_color;

        row = old_row;

        column = old_column;

    }

   

    void draw_box(size_t x, size_t y, size_t width, size_t height, uint8_t box_color) {

        // Top border

        VGA::BUFFER[y * VGA::WIDTH + x] = VGA::make_entry('+', box_color);

        for (size_t i = 1; i < width - 1; i++) {

            VGA::BUFFER[y * VGA::WIDTH + x + i] = VGA::make_entry('-', box_color);

        }

        VGA::BUFFER[y * VGA::WIDTH + x + width - 1] = VGA::make_entry('+', box_color);

       

        // Sides

        for (size_t i = 1; i < height - 1; i++) {

            VGA::BUFFER[(y + i) * VGA::WIDTH + x] = VGA::make_entry('|', box_color);

            VGA::BUFFER[(y + i) * VGA::WIDTH + x + width - 1] = VGA::make_entry('|', box_color);

        }

       

        // Bottom border

        VGA::BUFFER[(y + height - 1) * VGA::WIDTH + x] = VGA::make_entry('+', box_color);

        for (size_t i = 1; i < width - 1; i++) {

            VGA::BUFFER[(y + height - 1) * VGA::WIDTH + x + i] = VGA::make_entry('-', box_color);

        }

        VGA::BUFFER[(y + height - 1) * VGA::WIDTH + x + width - 1] = VGA::make_entry('+', box_color);

    }

};



// Global terminal instance

Terminal terminal;



// String conversion utilities

namespace StringUtil {

    void int_to_string(uint64_t value, char* buffer, int base = 10) {

        if (value == 0) {

            buffer[0] = '0';

            buffer[1] = '\0';

            return;

        }

       

        char temp[32];

        int i = 0;

       

        while (value > 0) {

            int digit = value % base;

            temp[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);

            value /= base;

        }

       

        int j = 0;

        while (i > 0) {

            buffer[j++] = temp[--i];

        }

        buffer[j] = '\0';

    }

}



// Draw a fancy GUI

void draw_gui() {

    terminal.clear();

   

    // Title bar

    terminal.set_color(VGA::WHITE, VGA::BLUE);

    for (size_t i = 0; i < VGA::WIDTH; i++) {

        terminal.putchar(' ');

    }

   

    const char* title = "MyOS64 - 64-bit Operating System v0.1";

    terminal.write_at((VGA::WIDTH - 40) / 2, 0, title, VGA::make_color(VGA::YELLOW, VGA::BLUE));

   

    // Main content area

    terminal.set_color(VGA::LIGHT_GREY, VGA::BLACK);

    terminal.row = 2;

    terminal.column = 0;

   

    // Welcome box

    terminal.draw_box(2, 2, 76, 8, VGA::make_color(VGA::CYAN, VGA::BLACK));

    terminal.write_at(5, 3, "Welcome to MyOS64!", VGA::make_color(VGA::LIGHT_GREEN, VGA::BLACK));

    terminal.write_at(5, 4, "A modern 64-bit operating system written from scratch", VGA::make_color(VGA::WHITE, VGA::BLACK));

   

    terminal.write_at(5, 6, "System Status:", VGA::make_color(VGA::YELLOW, VGA::BLACK));

    terminal.write_at(5, 7, "  [OK] CPU Mode: 64-bit Long Mode", VGA::make_color(VGA::LIGHT_GREEN, VGA::BLACK));

    terminal.write_at(5, 8, "  [OK] Kernel loaded successfully", VGA::make_color(VGA::LIGHT_GREEN, VGA::BLACK));

   

    // System info box

    terminal.draw_box(2, 11, 76, 6, VGA::make_color(VGA::MAGENTA, VGA::BLACK));

    terminal.write_at(5, 12, "System Information:", VGA::make_color(VGA::LIGHT_MAGENTA, VGA::BLACK));

    terminal.write_at(5, 13, "  Memory Model: Flat 64-bit", VGA::make_color(VGA::WHITE, VGA::BLACK));

    terminal.write_at(5, 14, "  Paging: Enabled (2MB huge pages)", VGA::make_color(VGA::WHITE, VGA::BLACK));

    terminal.write_at(5, 15, "  Display: VGA Text Mode 80x25", VGA::make_color(VGA::WHITE, VGA::BLACK));

   

    // Command prompt

    terminal.draw_box(2, 18, 76, 5, VGA::make_color(VGA::GREEN, VGA::BLACK));

    terminal.write_at(5, 19, "Command Prompt (Keyboard support coming soon):", VGA::make_color(VGA::LIGHT_GREEN, VGA::BLACK));

    terminal.write_at(5, 20, ">", VGA::make_color(VGA::YELLOW, VGA::BLACK));

   

    // Status bar at bottom

    terminal.set_color(VGA::BLACK, VGA::LIGHT_GREY);

    for (size_t i = 0; i < VGA::WIDTH; i++) {

        VGA::BUFFER[24 * VGA::WIDTH + i] = VGA::make_entry(' ', VGA::make_color(VGA::BLACK, VGA::LIGHT_GREY));

    }

    terminal.write_at(2, 24, "F1:Help", VGA::make_color(VGA::BLACK, VGA::LIGHT_GREY));

    terminal.write_at(12, 24, "F2:Info", VGA::make_color(VGA::BLACK, VGA::LIGHT_GREY));

    terminal.write_at(22, 24, "F10:Shutdown", VGA::make_color(VGA::BLACK, VGA::LIGHT_GREY));

    terminal.write_at(60, 24, "Ready", VGA::make_color(VGA::BLUE, VGA::LIGHT_GREY));

}



// Include interrupt and keyboard headers

#include "idt.h"

#include "keyboard.h"



// Forward declarations for ISR setup

extern "C" void init_pics();

extern "C" void handle_keyboard();



// Panic function for fatal errors

void panic(const char* message) {

    terminal.set_color(VGA::WHITE, VGA::RED);

    terminal.clear();

    terminal.write("KERNEL PANIC: ");

    terminal.write_line(message);

    terminal.write_line("");

    terminal.write_line("System Halted.");

   

    // Halt the system

    asm volatile("cli");

    while (1) {

        asm volatile("hlt");

    }

}



// Handle keyboard interrupt

extern "C" void handle_keyboard() {

    Keyboard::handle_interrupt();

}



// Command buffer

static char command_buffer[256];

static size_t command_pos = 0;



// Process command

void process_command() {

    command_buffer[command_pos] = '\0';

   

    // Move cursor to next line

    terminal.row = 21;

    terminal.column = 0;

   

    if (command_pos == 0) {

        terminal.row = 20;

        terminal.column = 2;

        return;

    }

   

    terminal.set_color(VGA::LIGHT_CYAN, VGA::BLACK);

   

    // Simple command processing

    if (command_buffer[0] == 'h' && command_buffer[1] == 'e' &&

        command_buffer[2] == 'l' && command_buffer[3] == 'p' && command_pos == 4) {

        terminal.write_line("Available commands:");

        terminal.write_line("  help    - Show this help");

        terminal.write_line("  clear   - Clear screen");

        terminal.write_line("  info    - System information");

        terminal.write_line("  hello   - Hello world");

    } else if (command_buffer[0] == 'c' && command_buffer[1] == 'l' &&

               command_buffer[2] == 'e' && command_buffer[3] == 'a' &&

               command_buffer[4] == 'r' && command_pos == 5) {

        draw_gui();

    } else if (command_buffer[0] == 'i' && command_buffer[1] == 'n' &&

               command_buffer[2] == 'f' && command_buffer[3] == 'o' && command_pos == 4) {

        terminal.write_line("MyOS64 - 64-bit Operating System");

        terminal.write_line("Version: 0.1 Alpha");

        terminal.write_line("Architecture: x86_64");

        terminal.write_line("Kernel: Custom microkernel");

    } else if (command_buffer[0] == 'h' && command_buffer[1] == 'e' &&

               command_buffer[2] == 'l' && command_buffer[3] == 'l' &&

               command_buffer[4] == 'o' && command_pos == 5) {

        terminal.write_line("Hello from MyOS64!");

    } else {

        terminal.write("Unknown command: ");

        terminal.write_line(command_buffer);

        terminal.write_line("Type 'help' for available commands");

    }

   

    // Reset command buffer

    command_pos = 0;

   

    // Wait a moment then redraw prompt

    for (volatile int i = 0; i < 10000000; i++);

    terminal.row = 20;

    terminal.column = 2;

    terminal.set_color(VGA::YELLOW, VGA::BLACK);

    terminal.write("> ");

}



// Kernel entry point

extern "C" void kernel_main() {

    // Initialize and draw GUI

    draw_gui();

   

    // Initialize PICs

    init_pics();

   

    // Initialize IDT and enable interrupts

    IDT::initialize();

   

    // Initialize keyboard

    Keyboard::initialize();

   

    // Set cursor position for command input

    terminal.row = 20;

    terminal.column = 2;

    terminal.set_color(VGA::YELLOW, VGA::BLACK);

   

    // Kernel loop - handle keyboard input

    while (1) {

        if (Keyboard::has_key()) {

            char c = Keyboard::get_char();

           

            if (c == '\n') {

                // Process command

                process_command();

            } else if (c == '\b') {

                // Backspace

                if (command_pos > 0) {

                    command_pos--;

                    if (terminal.column > 2) {

                        terminal.column--;

                        terminal.putchar(' ');

                        terminal.column--;

                    }

                }

            } else if (command_pos < sizeof(command_buffer) - 1) {

                // Add character to buffer and display

                command_buffer[command_pos++] = c;

                terminal.putchar(c);

            }

        }

       

        // Halt CPU until next interrupt

        asm volatile("hlt");

    }

}
