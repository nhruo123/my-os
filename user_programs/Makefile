BASE_BUILD_DIR := $(shell pwd)/../build/user_programs
SYSROOT := $(shell pwd)/../sysroot
PROGRAM_LIST := $(shell find . -mindepth 1 -maxdepth 1 -type d -printf '%f\n')
	
.PHONY: dirs clean all $(PROGRAM_LIST)

all: dirs $(PROGRAM_LIST)
	


dirs:
	mkdir -p $(BASE_BUILD_DIR)

clean:
	rm -rf $(BASE_BUILD_DIR)


$(PROGRAM_LIST):
	$(MAKE) -C $@ all SYSROOT="$(SYSROOT)" BASE_BUILD_DIR="$(BASE_BUILD_DIR)"