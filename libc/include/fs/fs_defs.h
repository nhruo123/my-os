#ifndef VF_DEF_H
#define VF_DEF_H 1

#include <stdint.h>
#include <stddef.h>

#define MAX_FILE_NAME_SIZE  256
#define MAX_FS_NAME_SIZE    256
#define MAX_MOUNTS          256
#define MAX_MOUNT_SIZE      20

#define FS_FILE             0x01
#define FS_DIRECTORY        0x02

#define MOUNT_SEPARTOR      ':'
#define FILE_SEPARTOR       '/'

typedef struct dir_entry_s {
    char filename[MAX_FILE_NAME_SIZE + 1];
} dir_entry_t;

typedef struct file_stats_s {
    uint32_t size;
} file_stats_t;


#endif