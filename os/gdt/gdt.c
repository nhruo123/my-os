#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <gdt/gdt.h>
#include <gdt/tss.h>

gdt_entry_t gdt_entries[GDT_SIZE];
gdt_pointer_t gdt_ptr;

tss_t kernel_tss;

void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_SIZE) - 1;
    gdt_ptr.base = (uint32_t)gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0);               // NULL
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xC); // KERNEL CODE
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xC); // KERNEL DATA

    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xC); // USER CODE
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xC); // USER DATA

    gdt_set_entry(5, &kernel_tss, (&kernel_tss + sizeof(tss_t)), 0xE9, 0x0); // TSS

    memset(&kernel_tss, 0, sizeof(tss_t));

    kernel_tss.ss0 = KERNEL_DATA_SEGMENT;
    kernel_tss.esp0 = 0;

    kernel_tss.cs = (USER_CODE_SEGMENT | 0b11);
    
    kernel_tss.ss = (USER_DATA_SEGMENT | 0b11);
    kernel_tss.ds = (USER_DATA_SEGMENT | 0b11);
    kernel_tss.es = (USER_DATA_SEGMENT | 0b11);
    kernel_tss.fs = (USER_DATA_SEGMENT | 0b11);
    kernel_tss.gs = (USER_DATA_SEGMENT | 0b11);

    flush_gdt(&gdt_ptr);
    flush_tss(TSS_DATA_SEGMENT | 0b11);
}

void set_kernel_esp(uint32_t esp)
{
    kernel_tss.esp0 = esp;
}

static void gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t flags)
{
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_mid = (base >> 16) & 0xFF;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;

    gdt_entries[index].limit_low = (limit & 0xFFFF);

    gdt_entries[index].limit_high = (limit >> 16) & 0x0F;

    gdt_entries[index].flags = flags & 0xF;

    gdt_entries[index].access_bytes = access_byte;
}
