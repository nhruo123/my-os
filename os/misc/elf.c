#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <fs/vfs.h>
#include <misc/elf.h>
#include <misc/system.h>
#include <mmnger/mmnger_virtual.h>
#include <mmnger/context_management.h>
#include <multitasking/task.h>

uint32_t exec(char *filename, uint32_t argc, char **argv)
{

    file_stats_t stats;
    void *lowest_program_location = 0;
    uint32_t program_size = 0;


    Elf32_Header *header = (Elf32_Header *)EFL_FILE_LOCATION;

    if ( stats_vfs(filename, &stats) != 0 )
    {
        return -1; // no file
    }

    // map space for the file
    for (uint32_t page = EFL_FILE_LOCATION; page <= EFL_FILE_LOCATION + stats.size; page += PAGE_SIZE)
    {
        vmmngr_alloc_page_and_phys(page, USER_FLAGS);
    }
    
    int read_count = read_vfs(filename, (char *)header, 0, stats.size);

    // TODO test if elf is for current os and machin
    if (header->e_ident[0] != ELFMAG0 ||
        header->e_ident[1] != ELFMAG1 ||
        header->e_ident[2] != ELFMAG2 ||
        header->e_ident[3] != ELFMAG3)
    {
        printf("invalid elf file \n");
        // free mapped space for the file
        for (uint32_t page = EFL_FILE_LOCATION; page <= EFL_FILE_LOCATION + stats.size; page += PAGE_SIZE)
        {
            vmmngr_free_page_and_phys(page);
        }
        return -1;
    }

    for (uint32_t i = 0; i < header->e_shentsize * header->e_shnum; i += header->e_shentsize)
    {

        Elf32_Shdr *header_entry = (Elf32_Shdr *)((void *)header + (header->e_shoff + i));

        if (header_entry->sh_addr < (uint32_t)lowest_program_location)
        {
            lowest_program_location = header_entry->sh_addr;
        }

        if ((header_entry->sh_addr + header_entry->sh_size) - (uint32_t)lowest_program_location > program_size)
        {
            program_size = header_entry->sh_addr + header_entry->sh_size - (uint32_t)lowest_program_location;
        }

        for (size_t page = 0; page < header_entry->sh_size; page += PAGE_SIZE)
        {
            vmmngr_alloc_page_and_phys(header_entry->sh_addr + page, USER_FLAGS);
        }

        if (header_entry->sh_type == SHT_NOBITS)
        {
            memset(header_entry->sh_addr, 0, header_entry->sh_size);
        }
        else
        {
            memcpy(header_entry->sh_addr, ((void *)header + header_entry->sh_offset), header_entry->sh_size);
        }
    }

    void *entry_point = header->e_entry;

    for (uint32_t page = EFL_FILE_LOCATION; page <= EFL_FILE_LOCATION + stats.size; page += PAGE_SIZE)
    {
        vmmngr_free_page_and_phys(page);
    }

    // allocat a stack for the new procces
    for (uint32_t page = USER_STACK_BOTTOM; page <= USER_STACK_TOP; page += PAGE_SIZE)
    {
        vmmngr_alloc_page_and_phys(page, USER_FLAGS);
    }

    uint32_t heap_start = (uint32_t)entry_point + program_size + PAGE_SIZE;
    if (heap_start % PAGE_SIZE != 0)
    {
        heap_start += PAGE_SIZE - (heap_start % PAGE_SIZE);
    }

    current_active_task->user_stack_top = USER_STACK_TOP;

    current_active_task->user_heap.is_heap_static = true;
    current_active_task->user_heap.is_read_only = false;
    current_active_task->user_heap.start_address = (void *)heap_start;
    current_active_task->user_heap.end_address = ((void *)heap_start + PAGE_SIZE * 2);
    current_active_task->user_heap.max_end_address = current_active_task->user_heap.end_address;

    for (size_t page = current_active_task->user_heap.start_address; page <= current_active_task->user_heap.end_address; page += PAGE_SIZE)
    {
        vmmngr_alloc_page_and_phys(page, USER_FLAGS);
    }

    heap_t *user_heap_self_mapped = self_map_heap(current_active_task->user_heap);

    memcpy(&current_active_task->user_heap, user_heap_self_mapped, sizeof(heap_t));

    uint32_t real_argc = argc + 1;
    char **real_argv = malloc_h(sizeof(char *) * (real_argc), &current_active_task->user_heap);

    char *file_name_arg = malloc_h(sizeof(char) * strlen(filename), &current_active_task->user_heap);
    strcpy(file_name_arg, filename);
    real_argv[0] = file_name_arg;

    for (size_t i = 1; i < real_argc; i++)
    {
        size_t currnt_arg_len = strlen(argv[i]);

        char *current_arg = malloc_h(sizeof(char) * currnt_arg_len, &current_active_task->user_heap);
        strcpy(current_arg, argv[i]);

        real_argv[i] = current_arg;
    }

    enter_user_space_program(entry_point, real_argc, real_argv, USER_STACK_TOP);

    return -1; // BAD SHIT ABOARD ( we shouldn't return to here )
}