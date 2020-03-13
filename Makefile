BUILD_DIR=build
OS=$(BUILD_DIR)/os/os.elf
DISK_IMG=os.iso

all: bootdisk

.PHONY: directories bootdisk  os

os:
	make -C os

bootdisk: os
	mkdir -p isodir/boot/grub
	cp $(OS) isodir/boot/os.elf
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(DISK_IMG) isodir


qemu:
	qemu-system-i386 -cdrom $(DISK_IMG) -gdb tcp::26000 -S

clean:
	make -C os clean
	rm $(DISK_IMG)

directories: 
	mkdir -p $(BUILD_DIR)
