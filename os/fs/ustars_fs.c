#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fs/vfs.h>
#include <fs/ustars_fs.h>
#include <string.h>
#include <multiboot.h>

static size_t oct2bin(char *str, size_t size)
{
    size_t n = 0;
    char *c = str;
    while (size-- > 0)
    {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

static uint32_t find_file_offset(char *filename, ustar_headr_t *file_header, disk_t *disk)
{
    size_t disk_offset = 0;

    ustar_headr_t current_header;

    disk->read(&current_header, disk_offset, sizeof(ustar_headr_t), disk);
    disk_offset += USTAR_BLOCK_SIZE;

    while (current_header.name[0] != '\0')
    {
        if (strcmp(current_header.name, filename) == 0)
        {
            memcpy(file_header, &current_header, sizeof(ustar_headr_t));
            return disk_offset;
        }

        size_t current_file_size = oct2bin(current_header.size, 11);

        disk_offset += current_file_size;

        if ((current_file_size % USTAR_BLOCK_SIZE) != 0)
        {
            disk_offset += USTAR_BLOCK_SIZE - (current_file_size % USTAR_BLOCK_SIZE);
        }

        disk->read(&current_header, disk_offset, sizeof(ustar_headr_t), disk);
        disk_offset += USTAR_BLOCK_SIZE;
    }

    memset(file_header, 0, sizeof(ustar_headr_t));
    return 0;
}

static uint32_t ustar_probe(disk_t *disk)
{
    if (disk->read != NULL)
    {
        char *buff = calloc(20, sizeof(char));

        disk->read(buff, 0x101, 7, disk);

        return memcmp(buff, USTAR_MAGIC, strlen(USTAR_MAGIC));
    }

    return -1;
}

static uint32_t ustar_read(char *filename, char *buffer, uint32_t offset, uint32_t size, disk_t *disk)
{
    ustar_headr_t file_header;

    uint32_t file_offset = find_file_offset(filename, &file_header, disk);

    if (file_offset == 0)
    {
        return 0;
    }

    disk->read(buffer, offset + file_offset, size, disk);
}
static uint32_t *ustar_readdir(char *filename, dir_entry_t *dir_entry, uint32_t index, disk_t *disk)
{
    ustar_headr_t file_header;

    size_t disk_offset = 0;

    ustar_headr_t current_header;

    disk->read(&current_header, disk_offset, sizeof(ustar_headr_t), disk);
    disk_offset += USTAR_BLOCK_SIZE;

    while (current_header.name[0] != '\0')
    {
        if ((memcmp(current_header.name, filename, strlen(filename)) == 0) && // dose the file name starts with the dir name
            (strcmp(current_header.name, filename) != 0))                     // is the file name diffrent form the dir name
        {
            char *first_instance_of_name = strpbrk(filename, current_header.name);
            if (strcmp("", filename) == 0 || strchr(first_instance_of_name, FILE_SEPARTOR) == NULL)
            {
                if (index == 0)
                {
                    if (strrchr(current_header.name, FILE_SEPARTOR) != NULL)
                    {
                        strcpy(dir_entry->filename, strrchr(current_header.name, FILE_SEPARTOR));
                    }
                    else
                    {
                        strcpy(dir_entry->filename, current_header.name);
                    }
                    return 1;
                }
                else
                {
                    index--;
                }
            }
        }

        size_t current_file_size = oct2bin(current_header.size, 11);

        disk_offset += current_file_size;

        if ((current_file_size % USTAR_BLOCK_SIZE) != 0)
        {
            disk_offset += USTAR_BLOCK_SIZE - (current_file_size % USTAR_BLOCK_SIZE);
        }

        disk->read(&current_header, disk_offset, sizeof(ustar_headr_t), disk);
        disk_offset += USTAR_BLOCK_SIZE;
    }

    return 0;
}

static uint32_t *ustar_stats(char *filename, file_stats_t *stats, disk_t *disk)
{
    ustar_headr_t file_header;

    uint32_t file_offset = find_file_offset(filename, &file_header, disk);

    if (file_offset == 0)
    {
        return -1;
    }

    stats->size = oct2bin(file_header.size, 11);

    return 0;
}

filesystem_t *create_ustar_fs(char *name)
{
    size_t name_len = strlen(name);
    if (name_len <= MAX_FS_NAME_SIZE)
    {
        filesystem_t *fs = calloc(1, sizeof(filesystem_t));
        memcpy(fs->name, name, name_len);

        fs->probe = ustar_probe;
        fs->read = ustar_read;
        fs->read_dir = ustar_readdir;
        fs->stats = ustar_stats;
    }
}