BUILD_DIR=../build/os
OS=$(BUILD_DIR)/os.elf

CFLAGS +=-ffreestanding -fno-pic -m32 -gdwarf-4 -ggdb3
ASMFLAGS +=-f elf -F dwarf -g

SRCS_DIRS := $(shell find . -type d)
OUT_DIRS := $(subst ./, $(BUILD_DIR)/, $(SRCS_DIRS))
OUT_DIRS := $(BUILD_DIR) $(OUT_DIRS)
OS_C_SRCS := $(shell find . -name "*.c")
OS_C_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_C_SRCS))
OS_ASM_SRCS := $(shell find . -name "*.asm")
OS_ASM_OBJS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(OS_ASM_SRCS))
OS_OBJS := $(OS_C_OBJS)  $(OS_ASM_OBJS)

.PHONY: directories

all: directories $(OS)

$(BUILD_DIR)/%.o : %.asm
	nasm $(ASMFLAGS) $< -o $@


$(BUILD_DIR)/%.o : %.c
	gcc $(CFLAGS) -c $< -o $@

$(OS): $(OS_OBJS)
	ld -m elf_i386 -nmagic -Tos.lds $(OS_OBJS) -o $@


directories: $(OUT_DIRS)

$(OUT_DIRS):
	mkdir -p $(OUT_DIRS)

clean:
	rm -r $(BUILD_DIR)


