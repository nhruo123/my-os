#include "../screen/screen.h"
#include "interrupts.h"
#include "../multiboot.h"

void main(multiboot_info_t* mbd, char * pmm, int block_count, int block_size) 
{
  terminalInit();
  kprint("Hello world, from my kernal!\n");


  kprint("Starting init idt....\n");

  init_idt();

  kprint("Done init idt\n");

  // int zero = 0/0;

  __asm__("int $0x20");


  kprint("halt...\n");
  for(;;) {
  	// int x = 1;
  	__asm__("hlt");
  }
}
