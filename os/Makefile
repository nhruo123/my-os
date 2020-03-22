BUILD_DIR=../build/os
SYSROOT := $(shell pwd)/../sysroot
OS=$(BUILD_DIR)/os.elf

CC := i686-elf-gcc --sysroot="$(SYSROOT)" -isystem=/usr/include
AR := i686-elf-ar

CFLAGS +=-mgeneral-regs-only -O2 -g -ffreestanding -Wall -Wextra -D__is_kernel -Iinclude
LIBS:=$(LIBS) -nostdlib -lk -lgcc
C_OBJS :=$(shell find . -type f -name "*.c")
ASM_OBJS := $(shell find . -type f -name "*.s")
C_OBJS := $(C_OBJS:.%.c=$(BUILD_DIR)%.o)
ASM_OBJS := $(ASM_OBJS:.%.s=$(BUILD_DIR)%.o)
OBJS := $(C_OBJS) $(ASM_OBJS)


.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .c .s

all: $(OS)

$(OS): $(OBJS) os.lds
	$(CC) -T os.lds -o $@ $(CFLAGS) $(OBJS) $(LIBS)
	grub-file --is-x86-multiboot $@


$(BUILD_DIR)/%.o : %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)

$(BUILD_DIR)/%.o : %.s
	$(LIBK_OBJS)
	$(CC) -MD -c $< -o $@ $(CFLAGS)


install: install-headers install-kernel


install-headers:
	mkdir -p $(SYSROOT)/usr/include/.
	cp -R include/. $(SYSROOT)/usr/include/.

install-kernel : dirs $(OS)
	cd $(BUILD_DIR)
	mkdir -p $(SYSROOT)/boot
	cp $(OS) $(SYSROOT)/boot/


dirs:
	mkdir -p $(BUILD_DIR)
	cp -R ./. $(BUILD_DIR)


clean:
	rm -r $(BUILD_DIR)