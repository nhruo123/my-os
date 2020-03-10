BUILD_DIR=../build/os
OS=$(BUILD_DIR)/os

CFLAGS+=-ffreestanding -fno-pic -m32 -gdwarf-4 -ggdb3

SRCS_DIRS := $(shell find . -type d)
OUT_DIRS := $(subst ./, $(BUILD_DIR)/, $(SRCS_DIRS))
OUT_DIRS := $(BUILD_DIR) $(OUT_DIRS)
OS_SRCS := $(shell find . -name "*.c")
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))

.PHONY: directories

all: directories $(OS)

	

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(OS): $(OS_OBJS)
	ld -m elf_i386 -nmagic -Tos.lds $(OS_OBJS) -o $@


directories: $(OUT_DIRS)

$(OUT_DIRS):
	mkdir -p $(OUT_DIRS)

clean:
	rm -r $(BUILD_DIR)


