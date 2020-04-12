#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <gdt/gdt.h>

gdt_entry_t gdt_entries[GDT_SIZE];
gdt_pointer_t gdt_ptr;

void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_SIZE) - 1;
    gdt_ptr.base = (uint32_t)gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0); // NULL 
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xC); // KERNEL CODE
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xC); // KERNEL DATA

    flush_gdt(&gdt_ptr);
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
