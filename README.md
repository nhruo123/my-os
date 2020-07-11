# My os

##### This is an educational project nothing here is production ready this whole project was made for the sole purpose of learning c and about operating system in general.

This project need to be compile with an [elf x86 cross compiler]( https://wiki.osdev.org/GCC_Cross-Compiler).

In order to compile the project you need to run the fowling command `make clean; make` (I know this is a bad make script I tried to be sneaky and share some stuff form libk into libc form a very early development stage).

If you want to run the os you need qemu and gdb installed on your machine and then running `make grab-qemu` for grab bios, or `make qemu` for qemu bios (I always use grab).

The repo contains an os.iso file that can be run on a vm, the os can be installed on a usb stick [as described here]( https://wiki.osdev.org/GRUB#USB_instructions).

After you boot into user space with disk `a` mounted, you can call the any command in the `user space programs` like this: `a:echo.elf hello_world` (the ram disk is a so we use him and then we run an elf file called echo.elf with the parameter hello world (btw backspace is bugged good luck typing it out)).

Huge thanks to `ofir` for the grate help with the project and the countless hours of being used as a rubber duck. 
