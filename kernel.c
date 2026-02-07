#include <stdint.h>
#include <stddef.h>

// Déclarations des fonctions externes
extern void terminal_initialize(void);
extern void terminal_writestring(const char* data);
extern void terminal_setcolor(uint8_t color);
extern void gui_initialize(void);
extern void gui_draw_desktop(void);
extern void graphics_init(void);
extern void graphics_draw_pixel(int x, int y, uint32_t color);
extern void graphics_clear_screen(uint32_t color);

// Variables globales
static int current_mode = 0; // 0 = terminal, 1 = GUI

// Port I/O
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Gestion du clavier
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

char get_key() {
    while (!(inb(KEYBOARD_STATUS_PORT) & 1));
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Conversion basique scancode -> ASCII
    static const char scancode_to_ascii[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
        0, ' '
    };
    
    if (scancode < sizeof(scancode_to_ascii)) {
        return scancode_to_ascii[scancode];
    }
    return 0;
}

// Timer simple
void sleep(uint32_t milliseconds) {
    for (volatile uint32_t i = 0; i < milliseconds * 1000; i++);
}

// Commandes du terminal
void handle_command(const char* cmd) {
    if (cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p') {
        terminal_writestring("\nCommandes disponibles:\n");
        terminal_writestring("  help   - Affiche cette aide\n");
        terminal_writestring("  clear  - Efface l'ecran\n");
        terminal_writestring("  gui    - Passe en mode graphique\n");
        terminal_writestring("  info   - Informations systeme\n");
        terminal_writestring("  reboot - Redemarrer\n");
    }
    else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r') {
        terminal_initialize();
    }
    else if (cmd[0] == 'g' && cmd[1] == 'u' && cmd[2] == 'i') {
        terminal_writestring("\nPassage en mode GUI...\n");
        sleep(500);
        current_mode = 1;
        gui_initialize();
        gui_draw_desktop();
    }
    else if (cmd[0] == 'i' && cmd[1] == 'n' && cmd[2] == 'f' && cmd[3] == 'o') {
        terminal_writestring("\nMyOS64 v1.0\n");
        terminal_writestring("Architecture: x86_64\n");
        terminal_writestring("Mode: Terminal\n");
        terminal_writestring("Memoire: 64 MB\n");
    }
    else if (cmd[0] == 'r' && cmd[1] == 'e' && cmd[2] == 'b' && cmd[3] == 'o') {
        terminal_writestring("\nRedemarrage...\n");
        sleep(1000);
        // Reset via keyboard controller
        outb(0x64, 0xFE);
    }
    else if (cmd[0] != 0) {
        terminal_writestring("\nCommande inconnue. Tapez 'help' pour l'aide.\n");
    }
}

// Boucle principale du terminal
void terminal_loop() {
    char command[256];
    int cmd_idx = 0;
    
    terminal_writestring("MyOS64> ");
    
    while (current_mode == 0) {
        char c = get_key();
        
        if (c == 0) continue;
        
        if (c == '\n') {
            command[cmd_idx] = '\0';
            terminal_writestring("\n");
            handle_command(command);
            cmd_idx = 0;
            terminal_writestring("\nMyOS64> ");
        }
        else if (c == '\b') {
            if (cmd_idx > 0) {
                cmd_idx--;
                terminal_writestring("\b \b");
            }
        }
        else if (cmd_idx < 255) {
            command[cmd_idx++] = c;
            char str[2] = {c, 0};
            terminal_writestring(str);
        }
    }
}

// Boucle principale du GUI
void gui_loop() {
    while (current_mode == 1) {
        char c = get_key();
        
        // ESC pour retourner au terminal
        if (c == 27) { // ESC scancode
            current_mode = 0;
            terminal_initialize();
            terminal_writestring("\nRetour au mode terminal.\n");
            return;
        }
        
        sleep(10);
    }
}

// Point d'entrée principal du kernel
void kernel_main(void) {
    // Initialiser le terminal
    terminal_initialize();
    
    // Message de bienvenue
    terminal_setcolor(0x0B); // Cyan clair
    terminal_writestring("====================================\n");
    terminal_writestring("     Bienvenue dans MyOS64!        \n");
    terminal_writestring("====================================\n");
    terminal_setcolor(0x07); // Blanc sur noir
    terminal_writestring("\nSysteme d'exploitation 64-bit\n");
    terminal_writestring("Version 1.0.0\n\n");
    terminal_writestring("Tapez 'help' pour voir les commandes disponibles.\n");
    terminal_writestring("Tapez 'gui' pour passer en mode graphique.\n\n");
    
    // Boucle principale
    while (1) {
        if (current_mode == 0) {
            terminal_loop();
        } else {
            gui_loop();
        }
    }
}
