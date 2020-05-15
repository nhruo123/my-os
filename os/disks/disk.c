#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <disks/disk.h>

disk_t **disks;
uint32_t last_disk_id;

void init_disks()
{
    last_disk_id = 0;
    disks = malloc(MAX_DISKS * sizeof(disk_t *));
    memset(disks, 0, MAX_DISKS * sizeof(disk_t *));
}

uint32_t register_disk(disk_t *disk)
{
    if (last_disk_id + 1 == MAX_DISKS)
    {
        return -1;
    }

    uint32_t new_disk_id = last_disk_id;
    last_disk_id++;

    disks[new_disk_id] = disk;

    return new_disk_id;
}

disk_t *get_disk_by_id(uint32_t disk_id)
{
    if (disk_id >= MAX_DISKS)
    {
        return NULL;
    }

    return disks[disk_id];
}