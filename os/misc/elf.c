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

    if (stats_vfs(filename, &stats) != 0)
    {
        return -1; // no file
    }

    // map space for the file
    for (uint32_t page = EFL_FILE_LOCATION; page <= EFL_FILE_LOCATION + stats.size; page += PAGE_SIZE)
    {
        vmmngr_free_page_and_phys(page); // TODO REMOVE THIS LINE THIS IS A TEMP FIX
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

    //save argc for later
    char **tmp_argv = calloc(argc, sizeof(char*));
    for(size_t i = 0; i < argc ; i++)
    {
        tmp_argv[i] = malloc(strlen(argv[i]) + 1);
        strcpy(tmp_argv[i], argv[i]);
    }

    // printf("started parsing headers at 0x%x, and there are %d headers\n", ((void *)header + header->e_shoff), header->e_shnum);
    for (uint32_t i = 0; i < (header->e_shentsize * header->e_shnum); i += header->e_shentsize)
    {

        Elf32_Shdr *header_entry = (Elf32_Shdr *)((void *)header + (header->e_shoff + i));
        // printf("%d)header_entry(0x%x) = { sh_name = 0x%x, sh_type = 0x%x, sh_addr = 0x%x, sh_offset = 0x%x, sh_size = 0x%x } \n", i, ((void *)header + (header->e_shoff + i)), header_entry->sh_name, header_entry->sh_type, header_entry->sh_addr, header_entry->sh_offset, header_entry->sh_size);

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
            vmmngr_free_page_and_phys(page); // TODO REMOVE THIS LINE THIS IS A TEMP FIX
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
        vmmngr_free_page_and_phys(page); // TODO REMOVE THIS LINE THIS IS A TEMP FIX
        vmmngr_alloc_page_and_phys(page, USER_FLAGS);
    }

    uint32_t heap_start = (uint32_t)entry_point + program_size + PAGE_SIZE;
    if (heap_start % PAGE_SIZE != 0)
    {
        heap_start += PAGE_SIZE - (heap_start % PAGE_SIZE);
    }

    heap_t user_heap = {0};

    current_active_task->user_stack_top = USER_STACK_TOP;

    user_heap.is_heap_static = true;
    user_heap.is_read_only = false;
    user_heap.start_address = (void *)heap_start;
    user_heap.end_address = ((void *)heap_start + PAGE_SIZE * 2);
    user_heap.max_end_address = user_heap.end_address;

    for (size_t page = user_heap.start_address; page <= user_heap.end_address; page += PAGE_SIZE)
    {
        vmmngr_free_page_and_phys(page); // TODO REMOVE THIS LINE THIS IS A TEMP FIX
        vmmngr_alloc_page_and_phys(page, USER_FLAGS);
    }

    // TODO make sure user cant change heap settings
    current_active_task->user_heap = self_map_heap(user_heap);

    uint32_t real_argc = argc + 1;
    char **real_argv = malloc_h(sizeof(char *) * (real_argc), current_active_task->user_heap);

    char *file_name_arg = malloc_h(sizeof(char) * strlen(filename), current_active_task->user_heap);
    strcpy(file_name_arg, filename);
    real_argv[0] = file_name_arg;

    for (size_t i = 1; i < real_argc; i++)
    {
        size_t currnt_arg_len = strlen(tmp_argv[i - 1]);

        char *current_arg = malloc_h(sizeof(char) * currnt_arg_len, current_active_task->user_heap);
        strcpy(current_arg, tmp_argv[i - 1]);

        real_argv[i] = current_arg;
    }

    uint32_t start_data;
    memcpy(&start_data, entry_point, 4);

    // frees argc for later
    for(size_t i = 0; i < argc ; i++)
    {
        free(tmp_argv[i]);
    }

    free(tmp_argv);

    enter_user_space_program(entry_point, real_argc, real_argv, USER_STACK_TOP);

    // printf("WE ARE AFTER ENTER USER SPACE!!\n");
    // abort();

    return -1; // BAD SHIT ABOARD ( we shouldn't return to here )
}