#include <stdint.h>
#include <stdlib.h>
#include <mmnger/mmnger_virtual.h>

#include <multiboot.h>

#define mmap_set(bit) \
    pmm[bit / 8] |= (1 << (bit % 8));

#define mmap_unset(bit) \
    pmm[bit / 8] &= ~(1 << (bit % 8));

extern uint32_t *_kernel_physical_end;
extern uint32_t *_kernel_physical_start;
extern uint32_t *VIRT_BASE;
extern uint32_t *_kernel_start;
extern uint32_t *_kernel_bootstrap_start;
extern uint32_t *_kernel_bootstrap_end;
extern uint32_t bootstrap_heap_start;
extern uint32_t bootstrap_heap_end;

uint8_t *pmm __attribute__((section(".bootstrap.data"))) = 0;
uint32_t block_count __attribute__((section(".bootstrap.data"))) = 0;
uint32_t block_size __attribute__((section(".bootstrap.data"))) = 4096;

// TODO THIS IS BUGGY REMOVE THIS PLZ
uint32_t __attribute__((section(".bootstrap"))) bootstrap_alloc_first()
{

    for (uint32_t i = 0; i < block_count / 8; i++)
        if (pmm[i] != 0XFF)
            for (uint32_t j = 0; j < 8; j++)
            {

                int bit = 1 << j;
                if (!(pmm[i] & bit))
                {
                    uint32_t frame = i * 8 + j;
                    mmap_set(frame);
                    uint32_t physical_adress = frame * block_size;
                    for (uint32_t i = 0; i < block_size; i++)
                    {
                        ((char *)physical_adress)[i] = 0;
                    }
                    return physical_adress;
                }
            }

    return -1;
}

void __attribute__((section(".bootstrap"))) bootstrap_map_pages(
    uint32_t physical_adress_start,
    uint32_t virtual_adress_start,
    size_t pages,
    uint32_t *pd,
    heap_t *self_mapped_heap)
{
    uint32_t address_mask = ~0xfff;
    uint32_t page_flags = 0x3;

    uint32_t *pt;
    size_t page_dir_index = virtual_adress_start >> 22 & 0x3FF;
    size_t page_table_index = virtual_adress_start >> 12 & 0x3FF;

    for (size_t mapped_pages = 0; mapped_pages < pages; mapped_pages++)
    {
        // change current pd every 1024 pages
        if ((mapped_pages % 1024) == 0)
        {
            if (mapped_pages != 0)
            {
                page_dir_index++;
                page_table_index = 0;
            }

            // if page_table is not present map it
            if ((pd[page_dir_index] & 0x1) != 1)
            {
                pt =
                    ((uint32_t * (*)(size_t, size_t, heap_t *))((void *)aligned_malloc_h - (void *)&VIRT_BASE))(PAGE_SIZE, PAGE_SIZE, self_mapped_heap);
                pt = ((void * (*)(void*, int, size_t *))((void *)memset - (void *)&VIRT_BASE))(pt, 0, PAGE_SIZE);
                pd[page_dir_index] = (((uint32_t)pt) & address_mask) | page_flags;
            }
            else
            {
                pt = (uint32_t *)(((uint32_t)pd[page_dir_index]) & (~0XFFF));
            }
        }

        pt[page_table_index] = ((physical_adress_start + (mapped_pages * 0x1000)) & address_mask) | page_flags;
        page_table_index++;
    }
}

void __attribute__((section(".bootstrap"))) init_bit_map(multiboot_info_t *mbt, heap_t *bootstrap_heap_out, char *pmm_out, int block_count_out, int block_size_out)
{
    heap_t static_heap = {0};
    static_heap.is_kernel_only = true;
    static_heap.is_read_only = false;
    static_heap.is_heap_static = true;
    static_heap.max_end_address = &bootstrap_heap_end;
    static_heap.start_address = &bootstrap_heap_start;
    static_heap.end_address = &bootstrap_heap_end;

    // Calls self_map_heap with VIRT_BASE offset
    heap_t *self_mapped_heap = ((heap_t * (*)(heap_t))((void *)self_map_heap - (void *)&VIRT_BASE))(static_heap);

    // allocate space for pmm with malloc_h from the real adress of the function before mapping
    multiboot_info_t *new_mbt = ((multiboot_info_t * (*)(size_t, heap_t *))((void *)malloc_h - (void *)&VIRT_BASE))(sizeof(multiboot_info_t), self_mapped_heap);

    // memcopy mbt to new mbt
    for (size_t i = 0; i < sizeof(multiboot_info_t); i++)
    {
        ((char *)new_mbt)[i] = ((char *)mbt)[i];
    }

    void *mmap_copy = ((void *(*)(size_t, heap_t *))((void *)malloc_h - (void *)&VIRT_BASE))(mbt->mmap_length, self_mapped_heap);

    // memcopy mbt to new mbt
    for (size_t i = 0; i < mbt->mmap_length; i++)
    {
        ((char *)mmap_copy)[i] = ((char *)mbt->mmap_addr)[i];
    }

    new_mbt->mmap_addr = (uint32_t)mmap_copy;

    uint32_t kernel_pages = ((uint32_t)&_kernel_physical_end - (uint32_t)&_kernel_physical_start) / PAGE_SIZE;
    uint32_t bootstrap_pages = ((uint32_t)&_kernel_bootstrap_end - (uint32_t)&_kernel_bootstrap_start) / PAGE_SIZE;

    uint32_t address_mask = ~0xfff;
    uint32_t page_flags = 0x3;

    uint32_t *pd =
        ((uint32_t * (*)(size_t, size_t, heap_t *))((void *)aligned_malloc_h - (void *)&VIRT_BASE))(PAGE_SIZE, PAGE_SIZE, self_mapped_heap);

    pd = ((void * (*)(void*, int, size_t *))((void *)memset - (void *)&VIRT_BASE))(pd, 0, PAGE_SIZE);

    pd[1023] = ((uint32_t)pd & address_mask) | page_flags; //self map

    bootstrap_map_pages((uint32_t)&_kernel_physical_start, (uint32_t)&_kernel_start, kernel_pages, pd, self_mapped_heap); // maps kernel

    bootstrap_map_pages((uint32_t)&_kernel_bootstrap_start, (uint32_t)&_kernel_bootstrap_start, bootstrap_pages, pd, self_mapped_heap); // maps bootsrap ljmp code

    bootstrap_map_pages(mbt->framebuffer_addr_lower, mbt->framebuffer_addr_lower, 1, pd, self_mapped_heap); // maps bootsrap ljmp code

    // bootstrap_map_pages(0xB8000, 0xB8000, 1, pd, self_mapped_heap); // maps bootsrap ljmp code

    asm("movl %0, %%eax; movl %%eax, %%cr3;" ::"r"(pd));

    mbt = new_mbt;
    bootstrap_heap_out = self_mapped_heap;
    pmm_out = pmm;
    block_count_out = block_count;
    block_size_out = block_size;
}