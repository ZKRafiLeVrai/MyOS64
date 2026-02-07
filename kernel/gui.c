#include <stdint.h>
#include <stddef.h>

// Déclarations externes
extern void graphics_clear_screen(uint32_t color);
extern void graphics_draw_rect(int x, int y, int width, int height, uint32_t color);
extern void graphics_draw_rect_border(int x, int y, int width, int height, 
                                     uint32_t fill, uint32_t border, int border_width);
extern void graphics_draw_circle(int cx, int cy, int radius, uint32_t color);
extern void graphics_get_screen_size(uint32_t* width, uint32_t* height);
extern void graphics_draw_pixel(int x, int y, uint32_t color);

// Couleurs Windows 11
#define COLOR_BACKGROUND    0x202020  // Gris foncé
#define COLOR_TASKBAR       0xF3F3F3  // Gris clair
#define COLOR_ACCENT        0x0078D4  // Bleu Windows
#define COLOR_WINDOW        0xFFFFFF  // Blanc
#define COLOR_BORDER        0xE0E0E0  // Bordure grise
#define COLOR_TEXT          0x000000  // Noir
#define COLOR_SHADOW        0x40000000 // Ombre semi-transparente
#define COLOR_START_HOVER   0xE0E0E0  // Survol bouton démarrer
#define COLOR_ICON_BG       0xF0F0F0  // Fond icône

// Dimensions
static uint32_t screen_w = 1024;
static uint32_t screen_h = 768;

// Initialiser le GUI
void gui_initialize(void) {
    graphics_get_screen_size(&screen_w, &screen_h);
}

// Dessiner un caractère simple (bitmap 8x8)
void gui_draw_char(int x, int y, char c, uint32_t color) {
    // Police bitmap très simple 8x8
    static const uint8_t font[128][8] = {
        // Espace (32)
        [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // ! (33)
        [33] = {0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00},
        // A (65)
        [65] = {0x18, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
        // B (66)
        [66] = {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},
        // C (67)
        [67] = {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},
        // D (68)
        [68] = {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},
        // E (69)
        [69] = {0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00},
        // M (77)
        [77] = {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00},
        // O (79)
        [79] = {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
        // S (83)
        [83] = {0x3E, 0x60, 0x60, 0x3C, 0x06, 0x06, 0x7C, 0x00},
        // Y (89)
        [89] = {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},
        // 0-9
        [48] = {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00}, // 0
        [49] = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, // 1
        [50] = {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00}, // 2
    };
    
    if (c < 0 || c >= 128) return;
    
    for (int dy = 0; dy < 8; dy++) {
        uint8_t row = font[(int)c][dy];
        for (int dx = 0; dx < 8; dx++) {
            if (row & (0x80 >> dx)) {
                graphics_draw_pixel(x + dx, y + dy, color);
            }
        }
    }
}

// Dessiner une chaîne
void gui_draw_string(int x, int y, const char* str, uint32_t color) {
    int pos_x = x;
    while (*str) {
        gui_draw_char(pos_x, y, *str, color);
        pos_x += 8;
        str++;
    }
}

// Dessiner la barre des tâches Windows 11
void gui_draw_taskbar(void) {
    int taskbar_height = 48;
    int taskbar_y = screen_h - taskbar_height;
    
    // Fond de la barre des tâches
    graphics_draw_rect(0, taskbar_y, screen_w, taskbar_height, COLOR_TASKBAR);
    
    // Bouton Démarrer (centré, style Windows 11)
    int start_btn_width = 48;
    int start_btn_x = screen_w / 2 - 200;
    int start_btn_y = taskbar_y + 6;
    
    // Fond arrondi du bouton (simulé avec rectangle)
    graphics_draw_rect_border(start_btn_x, start_btn_y, start_btn_width, 36,
                              COLOR_ICON_BG, COLOR_BORDER, 1);
    
    // Logo Windows (simplifié)
    int logo_x = start_btn_x + 12;
    int logo_y = start_btn_y + 8;
    int square_size = 8;
    int gap = 3;
    
    // 4 carrés pour simuler le logo Windows
    graphics_draw_rect(logo_x, logo_y, square_size, square_size, COLOR_ACCENT);
    graphics_draw_rect(logo_x + square_size + gap, logo_y, square_size, square_size, COLOR_ACCENT);
    graphics_draw_rect(logo_x, logo_y + square_size + gap, square_size, square_size, COLOR_ACCENT);
    graphics_draw_rect(logo_x + square_size + gap, logo_y + square_size + gap, square_size, square_size, COLOR_ACCENT);
    
    // Icônes d'applications (simulées)
    int icon_x = start_btn_x + start_btn_width + 10;
    for (int i = 0; i < 5; i++) {
        graphics_draw_rect_border(icon_x + i * 58, start_btn_y, 48, 36,
                                 COLOR_ICON_BG, COLOR_BORDER, 1);
        // Petit point pour représenter une icône
        graphics_draw_circle(icon_x + i * 58 + 24, start_btn_y + 18, 8, COLOR_ACCENT);
    }
    
    // Zone système (droite)
    int sys_tray_x = screen_w - 200;
    
    // Icônes système
    for (int i = 0; i < 3; i++) {
        graphics_draw_circle(sys_tray_x + i * 40 + 20, taskbar_y + 24, 6, COLOR_TEXT);
    }
    
    // Horloge
    gui_draw_string(screen_w - 80, taskbar_y + 10, "12:00", COLOR_TEXT);
    gui_draw_string(screen_w - 80, taskbar_y + 26, "01/01", COLOR_TEXT);
}

// Dessiner une fenêtre Windows 11
void gui_draw_window(int x, int y, int width, int height, const char* title) {
    // Ombre portée (effet de profondeur)
    graphics_draw_rect(x + 5, y + 5, width, height, 0x80808080);
    
    // Fond de la fenêtre
    graphics_draw_rect(x, y, width, height, COLOR_WINDOW);
    
    // Barre de titre
    int titlebar_height = 32;
    graphics_draw_rect(x, y, width, titlebar_height, COLOR_WINDOW);
    graphics_draw_rect(x, y + titlebar_height, width, 1, COLOR_BORDER);
    
    // Titre de la fenêtre
    gui_draw_string(x + 12, y + 12, title, COLOR_TEXT);
    
    // Boutons de contrôle (Minimiser, Maximiser, Fermer)
    int btn_width = 46;
    int btn_y = y + 0;
    
    // Minimiser
    int minimize_x = x + width - btn_width * 3;
    graphics_draw_rect(minimize_x, btn_y, btn_width, titlebar_height, COLOR_WINDOW);
    graphics_draw_rect(minimize_x + 16, btn_y + 20, 14, 1, COLOR_TEXT);
    
    // Maximiser
    int maximize_x = x + width - btn_width * 2;
    graphics_draw_rect(maximize_x, btn_y, btn_width, titlebar_height, COLOR_WINDOW);
    graphics_draw_rect_border(maximize_x + 16, btn_y + 10, 14, 12, 
                             COLOR_WINDOW, COLOR_TEXT, 1);
    
    // Fermer
    int close_x = x + width - btn_width;
    graphics_draw_rect(close_x, btn_y, btn_width, titlebar_height, 0xE81123);
    // X pour fermer
    for (int i = 0; i < 12; i++) {
        graphics_draw_pixel(close_x + 17 + i, btn_y + 10 + i, COLOR_WINDOW);
        graphics_draw_pixel(close_x + 17 + i, btn_y + 21 - i, COLOR_WINDOW);
    }
    
    // Bordure de la fenêtre
    graphics_draw_rect(x, y, width, 1, COLOR_BORDER);
    graphics_draw_rect(x, y, 1, height, COLOR_BORDER);
    graphics_draw_rect(x + width - 1, y, 1, height, COLOR_BORDER);
    graphics_draw_rect(x, y + height - 1, width, 1, COLOR_BORDER);
}

// Dessiner le bureau complet
void gui_draw_desktop(void) {
    // Fond d'écran (dégradé simulé)
    for (uint32_t y = 0; y < screen_h - 48; y++) {
        uint32_t shade = 0x20 + (y * 0x40) / screen_h;
        uint32_t color = (shade << 16) | (shade << 8) | (shade + 0x40);
        graphics_draw_rect(0, y, screen_w, 1, color);
    }
    
    // Fenêtre d'exemple "MyOS64"
    gui_draw_window(150, 100, 600, 400, "MyOS64 - Bienvenue");
    
    // Contenu de la fenêtre
    int content_y = 145;
    gui_draw_string(170, content_y, "Bienvenue dans MyOS64!", COLOR_ACCENT);
    gui_draw_string(170, content_y + 30, "Systeme d'exploitation 64-bit", COLOR_TEXT);
    gui_draw_string(170, content_y + 50, "Version 1.0.0", COLOR_TEXT);
    gui_draw_string(170, content_y + 80, "Fonctionnalites:", COLOR_TEXT);
    gui_draw_string(170, content_y + 100, "  - Mode terminal", COLOR_TEXT);
    gui_draw_string(170, content_y + 120, "  - Interface graphique", COLOR_TEXT);
    gui_draw_string(170, content_y + 140, "  - Architecture x86-64", COLOR_TEXT);
    gui_draw_string(170, content_y + 160, "  - Bootloader GRUB", COLOR_TEXT);
    
    gui_draw_string(170, content_y + 200, "Appuyez sur ESC pour retourner au terminal", 0x666666);
    
    // Deuxième fenêtre en arrière-plan
    gui_draw_window(400, 250, 500, 350, "Explorateur de fichiers");
    
    // Barre des tâches
    gui_draw_taskbar();
}

// Rafraîchir une zone
void gui_refresh(void) {
    gui_draw_desktop();
}