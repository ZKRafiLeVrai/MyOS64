# Outils
AS = nasm
CXX = g++
LD = ld

# Headers internes
INTERNAL_INCLUDES := -I$(shell $(CXX) -print-file-name=include)

# Flags
ASFLAGS = -f elf64
# -fno-stack-protector est CRITIQUE ici
# Change cette ligne dans ton Makefile
CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti \
         -nostdlib -nostdinc -mno-red-zone -mcmodel=small -fno-pic \
         -fno-stack-protector $(INTERNAL_INCLUDES)

CXXFLAGS = $(CFLAGS)

# Linker flags : -n (NMAGIC) et alignement des pages à 4Ko
LDFLAGS = -T linker.ld -nostdlib -n -z max-page-size=0x1000
# Dossiers et Fichiers
BUILD_DIR = build
KERNEL_DIR = kernel
ASM_SOURCES = $(KERNEL_DIR)/kernel_entry.asm $(KERNEL_DIR)/idt_asm.asm
CXX_SOURCES = $(wildcard $(KERNEL_DIR)/*.cpp)
OBJECTS = $(patsubst $(KERNEL_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES)) \
          $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SOURCES))

KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(BUILD_DIR)/myos.iso

all: $(ISO)

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(ISO): $(KERNEL)
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(KERNEL) $(BUILD_DIR)/iso/boot/kernel.bin
	cp grub.cfg $(BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(BUILD_DIR)/iso

clean:
	rm -rf $(BUILD_DIR)