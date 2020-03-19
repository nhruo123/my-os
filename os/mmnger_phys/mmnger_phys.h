#include <stdint.h>

//! 8 blocks per byte
#define PMMNGR_BLOCKS_PER_BYTE 8
 
//! block size (4k)
#define PMMNGR_BLOCK_SIZE	4096
 
//! block alignment
#define PMMNGR_BLOCK_ALIGN	PMMNGR_BLOCK_SIZE

//! size of physical memory
static	uint32_t	_mmngr_memory_size=0;
 
//! number of blocks currently in use
static	uint32_t	_mmngr_used_blocks=0;
 
//! maximum number of available memory blocks
static	uint32_t	_mmngr_max_blocks=0;
 
//! memory map bit array. Each bit represents a memory block
static	uint32_t*	_mmngr_memory_map= 0;