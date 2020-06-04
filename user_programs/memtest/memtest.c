#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <syscall_wrapper/syscall_wrapper.h>
#include <string.h>

#define PRESENT_PAGE        0b000000001
#define READ_WRITE_PAGE     0b000000010
#define USER_PAGE           0b000000100
#define PAGES_IN_TABLE      1024
#define KERNEL_START_ADDER	0xC0000000


typedef struct page_table_entry_s {
    uint32_t flags : 12;
    uint32_t physical_address : 20;
} page_table_entry_t;

typedef page_table_entry_t * page_table_t;

typedef struct page_dir_entry_s {
    uint32_t flags : 12;
    uint32_t physical_address : 20;
} page_dir_entry_t;

typedef page_dir_entry_t * page_directory_t;

typedef struct address_space_s {
    uint32_t physical_address;
} address_space_t;


static uint32_t get_page_address_from_indexes(uint32_t page_directory_index, uint32_t page_table_index)
{
    return ((page_directory_index << 22) + (page_table_index << 12));
}


void main(int argc, char **argv)
{
	for (size_t page_tbl = 1023; page_tbl < PAGES_IN_TABLE; page_tbl++)
	{
		page_dir_entry_t p_tbl = ((page_dir_entry_t *)get_page_address_from_indexes(1023, 1023))[page_tbl];
		if (((p_tbl.flags & PRESENT_PAGE) != 0) && ((p_tbl.flags & USER_PAGE) != 0))
		{
			printf("p_tbl(%d) : { flags: 0x%x, adder: 0x%x }\n", page_tbl, p_tbl.flags, p_tbl.physical_address);
		}

		for (size_t page = 0; (page < PAGES_IN_TABLE) && ((p_tbl.flags & PRESENT_PAGE) == 1); page++)
		{
			page_table_entry_t p = ((page_table_entry_t *)get_page_address_from_indexes(1023, page_tbl))[page];

			if (((p.flags & PRESENT_PAGE) != 0) && ((p.flags & USER_PAGE) != 0))
			{
				printf("page(%d,%d) : { flags: 0x%x, adder: 0x%x }\n",page_tbl, page, p.flags, p.physical_address);
			}
		}
	}
}