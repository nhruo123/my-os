#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fs/fs.h>
#include <fs/initrd_utars.h>
#include <string.h>
#include <multiboot.h>

inode_t *initrd_ustar_inodes = NULL;
dirent_t initrd_ustar_dirnet = {0};

ustar_header_ptr_t *ustar_headers = NULL;
uint32_t headers_count = 0;

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

// MEMORY LEACK MAKE HEAP CRASH
static uint32_t init_ustar_headers(void *memory_location)
{
    headers_count = 0;

    ustar_headr_t *current_header = (ustar_headr_t *)memory_location;
    

    while (current_header->name[0] != '\0')
    {
        headers_count++;

        ustar_header_ptr_t *new_ustar_headers = malloc(sizeof(ustar_header_ptr_t) * headers_count);

        if (ustar_headers != NULL)
        {
            memcpy(new_ustar_headers, ustar_headers, (sizeof(ustar_header_ptr_t) * headers_count) - 1);
            free(ustar_headers);
        }

        ustar_headers = new_ustar_headers;

        ustar_headers[headers_count - 1] = memory_location;

        uint32_t current_header_size = oct2bin(current_header->size, 11);

        void *new_header_adder = (void *)current_header;

        new_header_adder += (((current_header_size + 511) / 512) + 1) * 512;

        current_header = (ustar_headr_t *)new_header_adder;
    }

    return headers_count;
}

static uint32_t initrd_ustar_read(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    ustar_header_ptr_t curret_file = ustar_headers[node->index - 1];
    size_t curret_file_size = oct2bin(curret_file->size, 11);
    if (curret_file_size == 0)
    {
        return 0;
    }

    if (offset + size > curret_file_size)
    {
        size = curret_file_size - offset;
    }
    memcpy(buffer, ((void *)curret_file + 512), size);

    return size;
}

static dirent_t *initrd_ustar_readdir(inode_t *node, uint32_t index)
{
    if (index > headers_count || index < 0)
    {
        return NULL;
    }

    memcpy(initrd_ustar_dirnet.name, ustar_headers[index]->name, strlen(ustar_headers[index]->name));
    initrd_ustar_dirnet.inode_number = index;

    return &initrd_ustar_dirnet;
}

static inode_t *initrd_ustar_finddir(inode_t *node, char *name)
{
    if (strcmp(name, "/") == 0)
    {
        return &(initrd_ustar_inodes[0]);
    }
    else
    {
        for (size_t i = 0; i < headers_count; i++)
        {
            if (strcmp((name + 1), (ustar_headers[i]->name)) == 0)
            {
                return &(initrd_ustar_inodes[i + 1]);
            }
        }
    }

    return NULL;
}

inode_t *init_ustar_initrd(multiboot_info_t *mbt)
{
    void *memory_location;

    if (mbt->mods_count == 0)
    {
        printf("NO MODS FROM GRUB\n");
        abort();
    }

    multiboot_module_t *mods_array = mbt->mods_addr;

    bool found_initrd = false;
    for (size_t i = 0; i < mbt->mods_count; i++)
    {
        if (strcmp((char *)mods_array[i].cmdline, "initrd") == 0)
        {
            found_initrd == true;
            size_t initrd_size = (mods_array[i].mod_end - mods_array[i].mod_start);
            memory_location = malloc(initrd_size);
            memcpy(memory_location, (void *)mods_array[i].mod_start, initrd_size);
        }
    }

    init_ustar_headers(memory_location);

    initrd_ustar_inodes = malloc(sizeof(inode_t) * (headers_count + 1));

    memset(initrd_ustar_inodes, 0, sizeof(inode_t) * (headers_count + 1));


    initrd_ustar_inodes[0].node_type = FS_DIRECTORY;
    initrd_ustar_inodes[0].readdir = initrd_ustar_readdir;
    initrd_ustar_inodes[0].finddir = initrd_ustar_finddir;

    for (size_t header_index = 1; header_index <= headers_count; header_index++)
    {
        initrd_ustar_inodes[header_index].index = header_index;
        initrd_ustar_inodes[header_index].read = initrd_ustar_read;
        initrd_ustar_inodes[header_index].group_id = ustar_headers[header_index - 1]->gid;
        initrd_ustar_inodes[header_index].user_id = ustar_headers[header_index - 1]->uid;
        initrd_ustar_inodes[header_index].length = oct2bin(ustar_headers[header_index - 1]->size, 11);

    }

    return &(initrd_ustar_inodes[0]);
}