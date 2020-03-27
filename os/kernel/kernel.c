#include <stdio.h>
#include <mmnger/mmnger_virtual.h>
#include <mmnger/mmnger_phys.h>
#include "interrupts.h"
#include "../multiboot.h"

void main(multiboot_info_t* mbd, char * pmm, int block_count, int block_size) 
{
  terminalInit();
  pmmngr_init(block_count,block_size, pmm);
  printf("Hello world, from my kernal!\n");


  kprint("Starting init idt....\n");

  init_idt();

  kprint("Done init idt\n");
  

  void* block = pmmngr_alloc_block();
  void* virt_addr = (void *)0x4000000;

  vmmngr_alloc_page(virt_addr ,block, PRESENT_PAGE | READ_WRITE_PAGE);

  vmmngr_free_page(virt_addr);


  // int zero = 0/0;

  // __asm__("int $0x20");


  kprint("halt...\n");
  for(;;) {
  	// int x = 1;
  	__asm__("hlt");
  }
}
