#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>

inode_t *root_inode = NULL;

uint32_t read_fs(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->read != NULL)
    {
        return node->read(node, offset, size, buffer);
    }
    else
    {
        return 0;
    }
}

uint32_t write_fs(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    if (node->write != NULL)
    {
        node->write(node, offset, size, buffer);
    }
    else
    {
        return 0;
    }
}

void open_fs(inode_t *node)
{
    if (node->open != NULL)
    {
        return node->open(node);
    }
}

void close_fs(inode_t *node)
{
    if (node->close != NULL)
    {
        return node->close(node);
    }
}

dirent_t *readdir_fs(inode_t *node, uint32_t index)
{
    if (((node->node_type & FS_DIRECTORY) != 0) && (node->readdir != NULL))
    {
        return node->readdir(node, index);
    }
    else
    {
        return NULL;
    }
}

inode_t *finddir_fs(inode_t *node, char *name) {
    if (((node->node_type & FS_DIRECTORY) != 0) && (node->finddir != NULL))
    {
        return node->finddir(node, name);
    }
    else
    {
        return NULL;
    }
}