#ifndef DISK_H
#define DISK_H 1

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fs/vfs.h>

#define MAX_DISKS       64
#define MAX_DISK_NAME   256

typedef struct disk_s {
    char name[MAX_DISK_NAME + 1];
    uint32_t (* read)(char* buffer, uint32_t offset, uint32_t len, struct disk_s* disk);
    uint32_t (* write)(char* buffer, uint32_t offset, uint32_t len, struct disk_s* disk);
    uint32_t size;
    void* private;
    struct filesystem_s* fs;
} disk_t;

void init_disks();
uint32_t register_disk(disk_t* disk);
disk_t *get_disk_by_id(uint32_t disk_id);


#endif