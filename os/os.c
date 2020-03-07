#include "vga.h"
#include "interrupts.h"

void main() 
{
  terminalInit();
  print("Hello wrold, from my kernal!\n");


  print("Starting init idt....\n");

  init_idt();

  print("Done init idt\n");

  __asm__("int $0x20");

  print("halt...\n");
  for(;;) {
  	int x = 1;
  }
}
