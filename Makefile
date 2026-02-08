# MyOS64 Makefile

# Compiler and flags
AS = nasm
CC = gcc
CXX = g++
LD = ld

# Récupération automatique du chemin des headers internes du compilateur
INTERNAL_INCLUDES := -I$(shell $(CXX) -print-file-name=include)

ASFLAGS = -f elf64
# Ajout de -fno-pic et des INTERNAL_INCLUDES
CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti \
         -nostdlib -nostdinc -mno-red-zone -mcmodel=kernel -fno-pic \
         $(INTERNAL_INCLUDES)

CXXFLAGS = $(CFLAGS)
LDFLAGS = -T linker.ld -nostdlib -z noexecstack

# Directories
BUILD_DIR = build
KERNEL_DIR = kernel
ISO_DIR = $(BUILD_DIR)/iso
GRUB_DIR = $(ISO_DIR)/boot/grub

# Source files
ASM_SOURCES = $(KERNEL_DIR)/kernel_entry.asm $(KERNEL_DIR)/idt_asm.asm
CXX_SOURCES = $(KERNEL_DIR)/kernel_main.cpp \
              $(KERNEL_DIR)/idt.cpp \
              $(KERNEL_DIR)/isr.cpp \
              $(KERNEL_DIR)/keyboard.cpp

# Object files
ASM_OBJECTS = $(patsubst $(KERNEL_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
CXX_OBJECTS = $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(CXX_OBJECTS)

# Output
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(BUILD_DIR)/myos.iso

.PHONY: all clean iso run

all: $(ISO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS) linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(ISO): $(KERNEL) grub.cfg | $(ISO_DIR)
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	cp grub.cfg $(GRUB_DIR)/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 512M