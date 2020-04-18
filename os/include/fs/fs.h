#ifndef FS_H
#define FS_H 1

#include <stdint.h>
#include <stddef.h>

#define MAX_NAME_SIZE   256

#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08 // Is the file an active mountpoint?


typedef uint32_t (*read_type_t)(struct inode_s *, uint32_t, uint32_t, uint8_t *);   // read => inode, offset, size, buffer
typedef uint32_t (*write_type_t)(struct inode_s *, uint32_t, uint32_t, uint8_t *);  // write => inode, offset, size, buffer
typedef void (*open_type_t)(struct inode_s *);                                      // open => inode
typedef void (*close_type_t)(struct inode_s *);                                     // close => inode
typedef struct dirent_s *(*readdir_type_t)(struct inode_s *, uint32_t);             // readdir => inode , index
typedef struct inode_t * (*finddir_type_t)(struct inode_t*,char *name);               // find dir => inode , name

typedef struct inode_s
{
    // metadata
    uint32_t index;
    uint32_t premissions_mask;
    uint32_t user_id;
    uint32_t group_id;
    uint32_t node_type;
    uint32_t length; // length is in bytes

    // file funcs
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t  finddir;

    // prt to mount or symlinks
    struct inode_s* ptr;
} inode_t;

typedef struct dirent_s // One of these is returned by the readdir call, according to POSIX.
{
    char name[MAX_NAME_SIZE]; // Filename.
    uint32_t inode_number;    // Inode number. Required by POSIX.
    
} dirent_t;



extern inode_t *root_inode;

uint32_t read_fs(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(inode_t *node);
void close_fs(inode_t *node);
dirent_t* readdir_fs(inode_t *node, uint32_t index);
inode_t *finddir_fs(inode_t *node, char *name);


#endif