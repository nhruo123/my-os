#ifndef INITRD_USTAR_H
#define INITRD_USTAR_H 1

#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>
#include <multiboot.h>

#define USTAR_MAGIC             "ustar"
#define AUSTAR_FILE_TYPE        '0'            /* regular file */
#define BUSTAR_FILE_TYPE        '\0'           /* regular file */
#define USTAR_HARD_LINK_TYPE    '1'            /* hard link */
#define USTAR_SYMBOLIC_TYPE     '2'            /* sym link */
#define USTAR_CHAR_TYPE         '3'            /* character special */
#define USTAR_BLOCK_TYPE        '4'            /* block special */
#define USTAR_DORECPTRY_TYPE    '5'            /* directory */
#define USTAR_FIFO_TYPE         '6'            /* FIFO special */

typedef struct ustar_headr_s
{
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
} ustar_headr_t;

typedef ustar_headr_t *ustar_header_ptr_t;

static size_t oct2bin(char *str, size_t size);
static uint32_t init_ustar_headers(void * memory_location);

static uint32_t initrd_ustar_read(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
static dirent_t *initrd_ustar_readdir(inode_t *node, uint32_t index);
static inode_t *initrd_ustar_finddir(inode_t *node, char *name);


inode_t *init_ustar_initrd(multiboot_info_t *mbt);


#endif