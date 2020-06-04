PROGRAM_NAME := $(notdir ${shell pwd})
BUILD_DIR := $(BASE_BUILD_DIR)/$(PROGRAM_NAME)
OUT := $(BUILD_DIR)/$(PROGRAM_NAME).elf

CC := i686-elf-gcc --sysroot="$(SYSROOT)" -isystem=/usr/include 
AR := i686-elf-ar

CFLAGS +=-O0 -g -ffreestanding -Wall -Wextra -Iinclude 

C_OBJS :=$(shell find . -type f -name "*.c")
ASM_OBJS := $(shell find . -type f -name "*.s")
C_OBJS := $(C_OBJS:.%.c=$(BUILD_DIR)%.o)
ASM_OBJS := $(ASM_OBJS:.%.s=$(BUILD_DIR)%.o)
OBJS := $(C_OBJS) $(ASM_OBJS)

LIBS:=$(LIBS) -lc -lgcc


.PHONY: dirs clean all 
.SUFFIXES: .o .c .s

all: $(OUT)

$(OUT): dirs $(OBJS) $(BUILD_DIR)/user.lds
	echo $(OBJS)
	$(eval TMP := $(shell find $(BUILD_DIR) -type f -name "*.o" -printf "%f\n" | grep "[^crt0.o]"))
	cd $(BUILD_DIR); $(CC) $(TMP) -T $(BUILD_DIR)/user.lds -o $@ -O0 -ffreestanding -Wall -Wextra -Iinclude  $(LIBS)


%.o : %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)

%.o : %.s
	$(CC) -MD -c $< -o $@ $(CFLAGS)

dirs:
	mkdir -p $(BUILD_DIR)
	cp -R ./. $(BUILD_DIR)

clean:
	rm -r $(BUILD_DIR)