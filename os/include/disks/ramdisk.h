#ifndef RAMDISK_H
#define RAMDISK_H 1

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <disks/disk.h>
#include <multiboot.h>


typedef struct ramdisk_priv_s {
    uint32_t start_addr;
    uint32_t end_addr;
} ramdisk_priv_t;

disk_t* create_ram_disk(char* name, uint32_t start_addr, uint32_t end_addr);
disk_t *create_ram_disk_form_mbt(char *name, char *module_name, multiboot_info_t *mbt);

uint32_t read_ramdisk(char* buffer, uint32_t offset, uint32_t len, disk_t* ramdisk);
uint32_t write_ramdisk(char* buffer, uint32_t offset, uint32_t len, disk_t* ramdisk);


#endif