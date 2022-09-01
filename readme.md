32-bit operating system coded for i386 processors using c++, x86 assembly language and linker scripts. 
### Pre-requisites to generate .bin and .iso files: 
1. linux OS with pre-existing bootloader (like Grub). 
2. with the following tools installed: g++, binutils, libc6-dev-i386, virtualbox, grub-legacy, xorriso. (use `sudo apt-get install <package-name>`)
3. with i386 architecture support and elf32-i386 support. (use `objdump -i` and `objdump -x <any_file>.o` commands to verify the existence)

### Features:
1. Boot on a PC with linux's grub bootloader:
- Step 1. `make install` => `ls /boot/` => should have kernel.bin
- Step 2. `sudo <text-editor> /boot/grub/grub.cfg` => Add the following entry:
```
### BEGIN myOS ###
 menuentry 'myOS' {
 	multiboot /boot/kernel.bin
	boot
 }
### END myOS ###
```
- On starting the PC, grub bootloader will display the different OS you can load, one among them will be myOS, which you can select using arrow keys in the keyboard and by hitting enter to load it.  
2. Boot on a virtualbox with iso file already added to it: `make run`
3. `make stop` to stop the virtualbox from running the OS.
4. Global Descriptor Table(GDT), Memory Segments with kernel and user space divided into unused, null, code and data segements.
5. Software to communicate with hardware ports(via PIC - Port Interface card) and interrupt descriptor table(IDT) to handle interrupts accordingly from IO devices - timer(hardware clock), keyboard, mouse.
6. Keyboard and mouse driver code with required abstractions.
7. Software support for peripheral component interconnect(PCI) along with Base Address register(BAR).
8. Video Graphics Array(VGA) support.
9. Minimum GUI built upon VGA: draggable window, colored widgets, mouse cursor, window.
10. Multitasking capability.
11. A Heap and dynamic memory manager with allocation and deallocation mimicing malloc and free using first-fit methodology, implemented using doubly linked lists as book-keeping structures.

More to be follow...

### Flaws:
1. No separation of kernel and user space. ([this is how you achieve it](https://wiki.osdev.org/Getting_to_Ring_3))

### References:
1. [osdev.org](https://wiki.osdev.org/Main_Page)
2. [lowlevel.eu](http://www.lowlevel.eu/wiki/Lowlevel:Portal)
3. [WYOOS](https://www.youtube.com/playlist?list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6M)
4. Writing a Simple Operating System — from Scratch by Nick Blundell.
5. x86-64 Assembly Language Programming with Ubuntu by Ed Jorgensen, Ph.D.


