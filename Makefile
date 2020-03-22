BUILD_DIR=build
SYSROOT=sysroot
OS:=$(SYSROOT)/boot/os.elf
DISK_IMG=os.iso

all: bootdisk

.PHONY: directories bootdisk  os

os:
	make -C os install-headers
	make -C libc install-headers
	make -C libc install-libs
	make -C os install-kernel
	

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
	rm -r $(SYSROOT)

install-headers: 

