# --- Configuration des outils ---
AS = nasm
CC = gcc
CXX = g++
LD = ld

# Récupération automatique des headers internes du compilateur (essentiel pour <stdint.h> etc.)
INTERNAL_INCLUDES := -I$(shell $(CXX) -print-file-name=include)

# --- Flags de compilation ---
ASFLAGS = -f elf64

# CFLAGS expliqués :
# -mno-red-zone : Obligatoire en 64-bit pour le kernel (évite l'usage des 128 octets sous la pile)
# -fno-stack-protector : Désactive les canaris de pile (évite de crash sans runtime)
# -fno-pic : Désactive le code indépendant de la position (nécessaire pour un kernel chargé à 1Mo)
# -mcmodel=kernel : Optimise les adresses pour la mémoire haute
CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti \
         -nostdlib -nostdinc -mno-red-zone -mcmodel=kernel -fno-pic \
         -fno-stack-protector $(INTERNAL_INCLUDES)

CXXFLAGS = $(CFLAGS)

# --- Flags du Linker (CRITIQUE POUR VIRTUALBOX) ---
# -n : Désactive l'alignement de section dynamique (NMAGIC), évite les "trous" dans le binaire
# --no-dynamic-linker : Indique qu'il n'y a pas de loader d'OS
# -static : Force le linkage statique
LDFLAGS = -T linker.ld -nostdlib -z noexecstack -n --no-dynamic-linker -static

# --- Dossiers ---
BUILD_DIR = build
KERNEL_DIR = kernel
ISO_DIR = $(BUILD_DIR)/iso
GRUB_DIR = $(ISO_DIR)/boot/grub

# --- Fichiers Sources ---
ASM_SOURCES = $(KERNEL_DIR)/kernel_entry.asm $(KERNEL_DIR)/idt_asm.asm
CXX_SOURCES = $(KERNEL_DIR)/kernel_main.cpp \
              $(KERNEL_DIR)/idt.cpp \
              $(KERNEL_DIR)/isr.cpp \
              $(KERNEL_DIR)/keyboard.cpp

# --- Fichiers Objets ---
ASM_OBJECTS = $(patsubst $(KERNEL_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
CXX_OBJECTS = $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(CXX_OBJECTS)

# --- Cibles ---
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(BUILD_DIR)/myos.iso

.PHONY: all clean iso run

all: $(ISO)

# Création des répertoires de build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

# Compilation ASM
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Compilation C++
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkage final du kernel
$(KERNEL): $(OBJECTS) linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Création de l'ISO avec GRUB
$(ISO): $(KERNEL) grub.cfg | $(ISO_DIR)
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	cp grub.cfg $(GRUB_DIR)/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR)

# Lancement rapide sur QEMU pour test
run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 512M