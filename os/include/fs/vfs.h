#ifndef VFS_H
#define VFS_H 1

#include <stdint.h>
#include <stddef.h>
#include <disks/disk.h>
#include <fs/fs_defs.h>

typedef struct filesystem_s {
    char name[MAX_FILE_NAME_SIZE + 1];
    uint32_t (* probe)(struct disk_s *);
    uint32_t (* read)(char* filename, char* buffer, uint32_t offset, uint32_t size, struct disk_s * disk);
    uint32_t (* write)(char* filename, char* buffer, uint32_t offset, uint32_t size, struct disk_s * disk);
    uint32_t (* read_dir)(char* filename, dir_entry_t* buffer, uint32_t index, struct disk_s * disk); // buffer needs to be MAX_NAME_SIZE
    uint32_t (* mk_file)(char* filename, uint32_t file_type, struct disk_s * disk);
    uint32_t (* stats)(char* filename, struct file_stats_s* file_stats, struct disk_s * disk);

    void *private;

} filesystem_t ;

typedef struct mount_point_s {
    char location[MAX_MOUNT_SIZE + 1];
    struct disk_s* disk;
} mount_point_t ;


static uint32_t split_file_name(char *name, char *mount_name, char *file_name);

uint32_t read_vfs(char* file_path, char* buffer, uint32_t offset, uint32_t size);
uint32_t write_vfs(char* file_path, char* buffer, uint32_t offset, uint32_t size);
uint32_t readdir_vfs(char* file_path, dir_entry_t* dir_entry, uint32_t index);
uint32_t mk_file_vfs(char* file_path, uint32_t file_type);
uint32_t stats_vfs(char* file_path, struct file_stats_s* file_stats);



void init_vfs();
uint32_t count_mounts();
mount_point_t *get_mount_by_index(uint32_t index);
mount_point_t *get_mount_by_name(char *name);
uint32_t mount_disk(struct disk_s* disk, char* location);


#endif