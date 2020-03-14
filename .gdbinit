define hook-stop
    # Translate the segment:offset into a physical address
    printf "[%4x:%4x] ", $cs, $eip
end
set arch i8086 / set arch i386
layout asm
layout reg
focus cmd
set disassembly-flavor intel

target remote localhost:26000
symbol-file build/os/os.elf
b *(_start - 0xC0000000)
b main
