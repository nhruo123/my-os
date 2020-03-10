BUILD_DIR=build
BOOTLOADER=$(BUILD_DIR)/bootloader/bootloader.o
OS=$(BUILD_DIR)/os/os.elf
DISK_IMG=$(BUILD_DIR)/disk.img

all: bootdisk

.PHONY: directories bootdisk bootloader os

bootloader:
	make -C bootloader
os:
	make -C os
bootdisk: bootloader os
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=5120
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=1 seek=0
	dd conv=notrunc if=$(OS) of=$(DISK_IMG) bs=512 count=$$(($(shell stat --printf="%s" $(OS))/512)) seek=1

qemu:
	qemu-system-i386 -machine q35 -fda $(DISK_IMG) -gdb tcp::26000 -S

clean:
	make -C bootloader clean
	make -C os clean
	rm $(DISK_IMG)
	rmdir $(BUILD_DIR)

directories: 
	mkdir -p $(BUILD_DIR)