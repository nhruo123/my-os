#include <mmnger/mmnger_phys.h>
#include <mmnger/mmnger_virtual.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <multiboot.h>

//! size of physical memory
static uint32_t _mmngr_memory_size = 0;

//! size of physical memory
static uint32_t _mmngr_page_count = 0;

//! memory map bit array. Each bit represents a memory block
static char *physical_memory_map = NULL;

static uint32_t _mmngr_block_size = 0;

static size_t mmap_adress_to_bit(uint32_t adder)
{
	if (adder == 0)
	{
		return 0;
	}
	return (adder / PAGE_SIZE);
}

static uint32_t mmap_bit_to_address(size_t bit)
{
	if (bit == 0)
	{
		return 0;
	}
	return (bit * PAGE_SIZE);
}

static void mmap_set_bit(int bit)
{
	physical_memory_map[bit / 8] |= (1 << (bit % 8));
}

static void mmap_unset_bit(int bit)
{
	physical_memory_map[bit / 8] &= ~(1 << (bit % 8));
}

static bool mmap_test_bit(int bit)
{
	return physical_memory_map[bit / 8] & (1 << (bit % 8));
}
static uint32_t pmmngr_get_page_count()
{
	return _mmngr_page_count;
}

static uint32_t mmap_get_first_free_pages_bit(size_t pages)
{
	for (size_t bit = 0; bit < pmmngr_get_page_count() * 8; bit++)
	{
		if (!mmap_test_bit(bit))
		{
			bool is_size_good = true;
			for (size_t j = 0; j < pages; j++)
			{
				if (!mmap_test_bit(bit + j))
				{
					continue;
				}
				else
				{
					bit += j;
					is_size_good = false;
				}
			}

			if (is_size_good)
			{
				return bit;
			}
		}
	}
	return 0;
}

static uint32_t mmap_get_first_free_page()
{
	mmap_get_first_free_pages_bit(1);
}

void pmmngr_init(multiboot_info_t *mbt)
{

	_mmngr_block_size = PAGE_SIZE;
	multiboot_memory_map_t *entry = mbt->mmap_addr;
	size_t entry_index = 0;
	size_t last_phyiscal_adress = 0;
	while (entry < mbt->mmap_addr + mbt->mmap_length)
	{
		size_t entry_phyiscal_adress = entry->addr_low + entry->length_low;
		if (entry_phyiscal_adress > last_phyiscal_adress)
		{
			last_phyiscal_adress = entry_phyiscal_adress;
		}
		entry = (multiboot_memory_map_t *)((uint32_t)entry + entry->size + sizeof(entry->size));
	}

	printf("last_phyiscal_adress = 0x%x \n", last_phyiscal_adress);
	size_t last_page_bit = mmap_adress_to_bit(last_phyiscal_adress);
	size_t last_page_byte = last_page_bit / 8;
	if(last_page_bit % 8 != 0) {
		last_page_byte++;
	}

	printf("last_page_byte == %d \n", last_page_byte);
	physical_memory_map = (char *)malloc(last_page_byte);
	printf("physical_memory_map = 0x%x \n", physical_memory_map);

	if(physical_memory_map == NULL) {
		printf("cant find memory for physical_memory_map!!! \n");
		abort();
	}
	
	_mmngr_memory_size = last_page_byte * PAGE_SIZE;
	_mmngr_page_count = last_page_byte;
	
	memset(physical_memory_map, 0xff, last_page_byte);

	entry = mbt->mmap_addr;
	entry_index = 0;
	while (entry < mbt->mmap_addr + mbt->mmap_length)
	{
		printf("entry(%d) = { addr_low = 0x%x ; addr_high = 0x%x ; size_low = 0x%x ; size_high = 0x%x ; type = %d } \n",
			   entry_index, entry->addr_low, entry->addr_high, entry->length_low, entry->length_high, entry->type);

		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			bool is_entry_good = true;
			if (entry->addr_low == 0)
			{
				is_entry_good = false;
			}

			uint32_t alligned_memory_in_entry = entry->addr_low;
			size_t usable_memory_in_entry = entry->length_low;

			// fix memory for pmm bit map
			if (alligned_memory_in_entry % PAGE_SIZE != 0)
			{
				size_t memory_to_remove = alligned_memory_in_entry % PAGE_SIZE;

				alligned_memory_in_entry += memory_to_remove;

				if (usable_memory_in_entry < memory_to_remove)
				{
					is_entry_good = false;
				}

				usable_memory_in_entry -= memory_to_remove;
			}

			if (usable_memory_in_entry % PAGE_SIZE != 0)
			{
				usable_memory_in_entry -= usable_memory_in_entry % PAGE_SIZE;
			}

			if (usable_memory_in_entry < PAGE_SIZE)
			{
				is_entry_good = false;
			}

			if (is_entry_good)
			{
				pmmngr_free_pages(alligned_memory_in_entry, usable_memory_in_entry / PAGE_SIZE);
			}
		}
		entry = (multiboot_memory_map_t *)((uint32_t)entry + entry->size + sizeof(entry->size));
	}

	extern uint32_t *_kernel_physical_end;

	size_t kernel_size = (&_kernel_physical_end);

	for (size_t page = 0; page <= kernel_size; page += PAGE_SIZE)
	{
		mmap_set_bit(mmap_adress_to_bit(page));
	}
}

void *pmmngr_alloc_pages(size_t pages)
{

	int page_bit = mmap_get_first_free_pages_bit(pages);
	if (page_bit == 0)
	{
		return OUT_OF_MEMORY; //out of memory
	}

	for (int i = 0; i < pages; i++)
	{
		mmap_set_bit(page_bit + i);
	}

	return mmap_bit_to_address(page_bit);
}

void *pmmngr_alloc_page()
{
	return pmmngr_alloc_pages(1);
}

void pmmngr_free_pages(void *addr, size_t pages)
{

	for (int i = 0; i < pages; i++)
	{
		mmap_unset_bit(mmap_adress_to_bit(((uint32_t)addr) + (i * PAGE_SIZE)));
	}
}

void pmmngr_free_page(void *addr)
{
	pmmngr_free_pages(addr, 1);
}
