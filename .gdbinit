#set arch i8086 / set arch i386
layout asm
layout reg
focus cmd
layout src
set disassembly-flavor intel

target remote localhost:26000
symbol-file build/os/os.elf
#b init_bit_map
#b *0x1003bc
b main
