#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <fs/vfs.h>

mount_point_t **mounts;
uint32_t next_mount_id;

static uint32_t split_file_name(char *name, char *mount_name, char *file_name)
{
    char *split_location = strchr(name, MOUNT_SEPARTOR);

    if (split_location == NULL)
    {
        return -1;
    }

    strcpy(file_name,split_location + 1);
    memcpy(mount_name,name, split_location - name);

    if (strlen(mount_name) > MAX_MOUNT_SIZE)
    {
        return -1;
    }

    return 0;
}

uint32_t read_vfs(char *file_path, char *buffer, uint32_t offset, uint32_t size)
{
    char mount_name[MAX_MOUNT_SIZE] = {0};
    char file_name[MAX_FILE_NAME_SIZE] = {0};

    if (split_file_name(file_path, mount_name, file_name) != 0)
    {
        return -1;
    }

    mount_point_t *mount = get_mount_by_name(mount_name);

    if (mount == NULL)
    {
        return -1;
    }

    if (mount->disk->fs->read == NULL)
    {
        return -1;
    }

    return mount->disk->fs->read(file_name, buffer, offset, size, mount->disk);
}

uint32_t write_vfs(char *file_path, char *buffer, uint32_t offset, uint32_t size)
{
    char mount_name[MAX_MOUNT_SIZE] = {0};
    char file_name[MAX_FILE_NAME_SIZE] = {0};

    if (split_file_name(file_path, mount_name, file_name) != 0)
    {
        return -1;
    }

    mount_point_t *mount = get_mount_by_name(mount_name);

    if (mount == NULL)
    {
        return -1;
    }

    if (mount->disk->fs->write == NULL)
    {
        return -1;
    }

    return mount->disk->fs->write(file_name, buffer, offset, size, mount->disk);
}

uint32_t readdir_vfs(char *file_path, dir_entry_t *dir_entry, uint32_t index)
{
    char mount_name[MAX_MOUNT_SIZE] = {0};
    char dir_name[MAX_FILE_NAME_SIZE] = {0};

    if (split_file_name(file_path, mount_name, dir_name) != 0)
    {
        return -1;
    }

    mount_point_t *mount = get_mount_by_name(mount_name);

    if (mount == NULL)
    {
        return -1;
    }

    if (mount->disk->fs->read_dir == NULL)
    {
        return -1;
    }

    return mount->disk->fs->read_dir(dir_name, dir_entry, index, mount->disk);
}

uint32_t mk_file_vfs(char *file_path, uint32_t file_type)
{
    char mount_name[MAX_MOUNT_SIZE] = {0};
    char file_name[MAX_FILE_NAME_SIZE] = {0};

    if (split_file_name(file_path, mount_name, file_name) != 0)
    {
        return -1;
    }

    mount_point_t *mount = get_mount_by_name(mount_name);

    if (mount == NULL)
    {
        return -1;
    }

    if (mount->disk->fs->mk_file == NULL)
    {
        return -1;
    }

    return mount->disk->fs->mk_file(file_name, file_type, mount->disk);
}

uint32_t mount_disk(disk_t *disk, char *location)
{
    if (next_mount_id >= MAX_MOUNTS )
    {
        return -1;
    }

    if (strlen(location) > MAX_MOUNT_SIZE)
    {
        return -1;
    }

    mount_point_t *new_mout = malloc(sizeof(mount_point_t));
    memset(new_mout, 0, sizeof(mount_point_t));

    new_mout->disk = disk;
    memcpy(new_mout->location, location, MAX_MOUNT_SIZE);

    mounts[next_mount_id] = new_mout;

    next_mount_id++;


    return 0;
}

mount_point_t *get_mount_by_index(uint32_t index)
{
    if (index >= next_mount_id)
    {
        return NULL;
    }
    else
    {
        return mounts[index];
    }
}

mount_point_t *get_mount_by_name(char *name)
{
    for (size_t i = 0; i < next_mount_id; i++)
    {
        if (strcmp(mounts[i]->location, name) == 0)
        {
            return mounts[i];
        }
    }

    return NULL;
}

uint32_t count_mounts()
{
    return next_mount_id;
}

void init_vfs()
{
    next_mount_id = 0;
    mounts = malloc(sizeof(mount_point_t *) * MAX_MOUNTS);
}