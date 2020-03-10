BUILD_DIR=../build/bootloader

BOOTLOADER_SRCS := $(wildcard *.asm)

BOOTLOADER_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(BOOTLOADER_SRCS))

.PHONY: directories


all: directories $(BOOTLOADER_OBJS)

$(BUILD_DIR)/%.o: %.asm
	nasm -f elf $< -F dwarf -g -o $@
	ld -m elf_i386 -T bootloader.lds $@ -o $@.elf
	objcopy -O binary $(BUILD_DIR)/bootloader.o.elf $@

clean:
	rm -r $(BUILD_DIR)

directories:
	mkdir -p $(BUILD_DIR)