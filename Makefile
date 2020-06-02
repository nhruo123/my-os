BUILD_DIR=build
SYSROOT=sysroot
OS:=$(SYSROOT)/boot/os.elf
DISK_IMG=os.iso
FS_DIR=fs_dir
FS_FILE=fs_disk.tar

all: bootdisk

.PHONY: directories bootdisk  os

os:
	make -C os install-headers
	make -C libc install-headers
	make -C libc install-libs
	make -C os install-kernel
	

bootdisk: os  $(FS_FILE)
	mkdir -p isodir/boot/grub
	cp $(OS) isodir/boot/os.elf
	cp $(FS_FILE) isodir/boot/$(FS_FILE)
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(DISK_IMG) isodir


grub-qemu:
	qemu-system-i386 -cdrom $(DISK_IMG) -gdb tcp::26000 -S

qemu:
	qemu-system-i386 -kernel $(OS) -gdb tcp::26000 -S

clean:
	make -C os clean
	make -C libc clean
	make -C user_programs clean
	rm -rf $(FS_FILE)
	rm -rf $(FS_FILE)
	find $(FS_DIR) -name "*.elf" -type f -delete
	rm -rf $(DISK_IMG)
	rm -rf $(SYSROOT)

compile-user: os
	make -C user_programs all

$(FS_FILE): compile-user
	mkdir -p $(FS_DIR)
	mkdir -p $(BUILD_DIR)/user_programs
	find $(BUILD_DIR)/user_programs/ -name \*.elf -exec cp {} $(FS_DIR) \;
	cd $(FS_DIR); tar -cvf $(FS_FILE) *
	mv $(FS_DIR)/$(FS_FILE) $(FS_FILE)

