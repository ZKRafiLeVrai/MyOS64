# MyOS64 Makefile

# Compiler and flags
AS = nasm
CC = x86_64-elf-gcc
CXX = x86_64-elf-g++
LD = x86_64-elf-ld

ASFLAGS = -f elf64
CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -nostdinc -mno-red-zone -mcmodel=kernel
CXXFLAGS = $(CFLAGS)
LDFLAGS = -T linker.ld -nostdlib

# Directories
BUILD_DIR = build
KERNEL_DIR = kernel
ISO_DIR = $(BUILD_DIR)/iso
GRUB_DIR = $(ISO_DIR)/boot/grub

# Source files
ASM_SOURCES = $(KERNEL_DIR)/kernel_entry.asm $(KERNEL_DIR)/idt_asm.asm
C_SOURCES =
CXX_SOURCES = $(KERNEL_DIR)/kernel_main.cpp \
              $(KERNEL_DIR)/idt.cpp \
              $(KERNEL_DIR)/isr.cpp \
              $(KERNEL_DIR)/keyboard.cpp

# Object files
ASM_OBJECTS = $(patsubst $(KERNEL_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
CXX_OBJECTS = $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS) $(CXX_OBJECTS)

# Output
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(BUILD_DIR)/myos.iso

.PHONY: all clean iso run

all: $(ISO)

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

# Compile assembly files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Compile C files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link kernel
$(KERNEL): $(OBJECTS) linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Create ISO
$(ISO): $(KERNEL) grub.cfg | $(ISO_DIR)
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	cp grub.cfg $(GRUB_DIR)/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Run in QEMU (optional)
run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 512M
