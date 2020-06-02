BUILD_DIR=../build/libc
SYSROOT := $(shell pwd)/../sysroot

CC := i686-elf-gcc --sysroot="$(SYSROOT)" -isystem=/usr/include
AR := i686-elf-ar

CFLAGS +=-O0 -g -ffreestanding -Wall -Wextra -D__is_libc -Iinclude
LIBK_CFLAGS := $(CFLAGS) -D__is_libk

BINARIES := $(BUILD_DIR)/libk.a $(BUILD_DIR)/libc.a

FREEOBJS :=$(shell find . -type f -name "*.c") $(shell find . -type f -name "*.s")
LIBC_OBJS := $(FREEOBJS:.%.c=$(BUILD_DIR)%.libc.o)

LIBK_OBJS := $(FREEOBJS:.%.c=$(BUILD_DIR)%.libk.o)
LIBK_OBJS := $(LIBK_OBJS:.s=.o)

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .libc.o .c .s

$(BUILD_DIR)/libk.a :$(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

$(BUILD_DIR)/libc.a :$(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)

# $(BUILD_DIR)/%.o : %.c
# 	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)

# $(BUILD_DIR)/%.o : %.s
# 	$(CC) -MD -S -masm=intel -c $< -o $@ $@ $(CFLAGS)

$(BUILD_DIR)/%.libk.o : %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS)

$(BUILD_DIR)/%.libk.o: %.s
	$(CC) -MD -S -masm=intel -c $< -o $@ $(LIBK_CFLAGS)

$(BUILD_DIR)/%.libc.o : %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)

$(BUILD_DIR)/%.libc.o: %.s
	$(CC) -MD -S -masm=intel -c $< -o $@ $(CFLAGS)


install-headers:
	mkdir -p $(SYSROOT)/usr/include/.
	cp -R include/. $(SYSROOT)/usr/include/.

install-libs : dirs $(BINARIES)
	cd $(BUILD_DIR)
	mkdir -p $(SYSROOT)/usr/lib
	cp $(BINARIES) $(SYSROOT)/usr/lib/

dirs:
	mkdir -p $(BUILD_DIR)
	cp -R ./. $(BUILD_DIR)

install: install-headers install-libs

clean:
	rm -rf $(BUILD_DIR)
