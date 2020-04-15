#ifndef GDT_H
#define GDT_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <gdt/tss.h>

#define GDT_SIZE                6

#define NULL_SEGMENT            0
#define KERNEL_CODE_SEGMENT     0x8
#define KERNEL_DATA_SEGMENT     0x10
#define USER_CODE_SEGMENT       0x18
#define USER_DATA_SEGMENT       0x20
#define TSS_DATA_SEGMENT        0x28

struct gdt_entry_s {
    uint16_t limit_low : 16;
    uint16_t base_low : 16; // first word end
    uint8_t  base_mid : 8;
    uint8_t  access_bytes : 8;
    uint8_t  limit_high : 4;
    uint8_t  flags : 4;
    uint8_t  base_high : 8;
} __attribute__((packed));

typedef struct gdt_entry_s gdt_entry_t;


struct gdt_pointer_s
{
   uint16_t limit;
   uint32_t base;
} __attribute__((packed));
typedef struct gdt_pointer_s gdt_pointer_t;
 


void init_gdt();
static void gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access_byte , uint8_t flags);
extern void flush_gdt(gdt_pointer_t *gdt_ptr);


#endif