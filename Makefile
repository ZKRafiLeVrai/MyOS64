# Makefile pour MyOS64

# Compilateurs et outils
ASM = nasm
CC = gcc
LD = ld
GRUB_MKRESCUE = grub-mkrescue

# Flags
ASMFLAGS = -f elf64
CFLAGS = -m64 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -mno-red-zone
LDFLAGS = -m elf_x86_64 -T linker.ld -nostdlib

# Répertoires
BOOT_DIR = boot
KERNEL_DIR = kernel
BUILD_DIR = build
ISO_DIR = iso

# Fichiers sources
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_ENTRY_SRC = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_SRCS = $(KERNEL_DIR)/kernel.c $(KERNEL_DIR)/terminal.c $(KERNEL_DIR)/graphics.c $(KERNEL_DIR)/gui.c

# Fichiers objets
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJS = $(BUILD_DIR)/kernel.o $(BUILD_DIR)/terminal.o $(BUILD_DIR)/graphics.o $(BUILD_DIR)/gui.o

# Fichier binaire final
KERNEL_BIN = myos.bin
ISO_FILE = MyOS64.iso

# Couleurs pour les messages
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

.PHONY: all clean run iso dirs

all: dirs $(ISO_FILE)
	@echo "$(GREEN)✓ Build terminé avec succès!$(NC)"
	@echo "$(YELLOW)Utilisez 'make run' pour tester avec QEMU$(NC)"

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(ISO_DIR)/boot/grub

# Compiler le bootloader
$(BOOT_OBJ): $(BOOT_SRC)
	@echo "$(BLUE)Compilation du bootloader...$(NC)"
	$(ASM) $(ASMFLAGS) $< -o $@

# Compiler le point d'entrée du kernel
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC)
	@echo "$(BLUE)Compilation du kernel entry...$(NC)"
	$(ASM) $(ASMFLAGS) $< -o $@

# Compiler les fichiers C du kernel
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	@echo "$(BLUE)Compilation de $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

# Lier tous les objets
$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJS)
	@echo "$(BLUE)Liaison du kernel...$(NC)"
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "$(GREEN)✓ Kernel créé$(NC)"

# Créer l'ISO bootable avec GRUB
$(ISO_FILE): $(KERNEL_BIN)
	@echo "$(BLUE)Création de l'ISO bootable...$(NC)"
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp grub.cfg $(ISO_DIR)/boot/grub/
	$(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR)/ 2>/dev/null || $(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR)/
	@echo "$(GREEN)✓ ISO créé: $(ISO_FILE)$(NC)"

# Tester avec QEMU
run: $(ISO_FILE)
	@echo "$(YELLOW)Démarrage de QEMU...$(NC)"
	qemu-system-x86_64 -cdrom $(ISO_FILE) -m 512M

# Tester avec QEMU en mode debug
debug: $(ISO_FILE)
	@echo "$(YELLOW)Démarrage de QEMU en mode debug...$(NC)"
	qemu-system-x86_64 -cdrom $(ISO_FILE) -m 512M -s -S

# Nettoyer les fichiers de build
clean:
	@echo "$(YELLOW)Nettoyage...$(NC)"
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)
	rm -f $(KERNEL_BIN)
	rm -f $(ISO_FILE)
	@echo "$(GREEN)✓ Nettoyage terminé$(NC)"

# Afficher les informations
info:
	@echo "$(BLUE)=== MyOS64 Build System ===$(NC)"
	@echo "Bootloader: $(BOOT_SRC)"
	@echo "Kernel sources: $(KERNEL_SRCS)"
	@echo "Output: $(KERNEL_BIN) -> $(ISO_FILE)"
	@echo ""
	@echo "$(YELLOW)Commandes disponibles:$(NC)"
	@echo "  make         - Compiler l'OS"
	@echo "  make run     - Compiler et exécuter avec QEMU"
	@echo "  make debug   - Compiler et exécuter en mode debug"
	@echo "  make clean   - Nettoyer les fichiers de build"
	@echo "  make info    - Afficher ces informations"

# Aide
help: info
