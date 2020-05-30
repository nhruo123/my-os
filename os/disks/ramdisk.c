#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <disks/disk.h>
#include <disks/ramdisk.h>
#include <string.h>
#include <multiboot.h>

disk_t *create_ram_disk(char *name, uint32_t start_addr, uint32_t end_addr)
{
    ramdisk_priv_t *disk_priv = calloc(1, sizeof(ramdisk_priv_t));

    disk_priv->end_addr = (void *)end_addr;
    disk_priv->start_addr = (void *)start_addr;

    disk_t *new_disk = calloc(1, sizeof(disk_t));

    memcmp(new_disk->name, name, MAX_DISK_NAME);
    
    new_disk->size = (uint32_t)(end_addr - start_addr);
    new_disk->read = read_ramdisk;
    new_disk->write = write_ramdisk;
    new_disk->private = disk_priv;

    return new_disk;
}

uint32_t read_ramdisk(char *buffer, uint32_t offset, uint32_t len, disk_t *ramdisk)
{
    ramdisk_priv_t *this_priv = (ramdisk_priv_t *)(ramdisk->private);
    uint32_t start_addr = offset + this_priv->start_addr;
    uint32_t read_amount = 0;

    if (start_addr > this_priv->end_addr)
    {
        return read_amount;
    }
    else if (start_addr + len > this_priv->end_addr)
    {
        read_amount = len - ((start_addr + len) - this_priv->end_addr);
        memcpy(buffer, this_priv->start_addr + offset, read_amount);
        return read_amount;
    }
    else
    {
        read_amount = len;
        memcpy(buffer, this_priv->start_addr + offset, read_amount);
        return read_amount;
    }
}

disk_t *create_ram_disk_form_mbt(char *name, char *module_name, multiboot_info_t *mbt)
{ 
    void *memory_location;
    size_t initrd_size;

    if (mbt->mods_count == 0)
    {
        printf("NO MODS FROM GRUB\n");
        abort();
    }

    multiboot_module_t *mods_array = mbt->mods_addr;

    bool found_initrd = false;
    for (size_t i = 0; i < mbt->mods_count; i++)
    {
        if (strcmp((char *)mods_array[i].cmdline, module_name) == 0)
        {
            found_initrd == true;
            initrd_size = (mods_array[i].mod_end - mods_array[i].mod_start);
            memory_location = malloc(initrd_size);
            memcpy(memory_location, (void *)mods_array[i].mod_start, initrd_size);
        }
    }

    if(found_initrd) 
    {
        printf("NO %s MOD FROM GRUB\n", module_name);
        abort();
    }

    return create_ram_disk(name, (uint32_t)memory_location, ((uint32_t)memory_location) + initrd_size);
}

uint32_t write_ramdisk(char *buffer, uint32_t offset, uint32_t len, disk_t *ramdisk)
{
    ramdisk_priv_t *this_priv = (ramdisk_priv_t *)(ramdisk->private);
    uint32_t start_addr = offset + this_priv->start_addr;
    uint32_t read_amount = 0;

    if (start_addr > this_priv->end_addr)
    {
        return read_amount;
    }
    else if (start_addr + len > this_priv->end_addr)
    {
        read_amount = len - ((start_addr + len) - this_priv->end_addr);
        memcpy(this_priv->start_addr + offset, buffer, read_amount);
        return read_amount;
    }
    else
    {
        read_amount = len;
        memcpy(this_priv->start_addr + offset, buffer, read_amount);
        return read_amount;
    }
}
