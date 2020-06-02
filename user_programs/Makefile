BASE_BUILD_DIR := $(shell pwd)/../build/user_programs
SYSROOT := $(shell pwd)/../sysroot
PROGRAM_LIST := $(shell find . -mindepth 1 -maxdepth 1 -type d -printf '%f\n')
	
.PHONY: dirs clean all 

all: dirs
	make -C $(PROGRAM_LIST) all SYSROOT="$(SYSROOT)" BASE_BUILD_DIR="$(BASE_BUILD_DIR)"


dirs:
	mkdir -p $(BASE_BUILD_DIR)

clean:
	make -C $(PROGRAM_LIST) clean SYSROOT="$(SYSROOT)" BASE_BUILD_DIR="$(BASE_BUILD_DIR)"
	rm -r $(BASE_BUILD_DIR)