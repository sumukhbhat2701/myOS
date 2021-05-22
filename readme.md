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
- On starting the PC, grub bootloader will display the different OS you can load, one among them will be <some_name> which you can select using arrow keys in the keyboard and by hitting enter.  
2. Boot on a virtualbox with iso file already added to it: `make run`
3. `make stop` to stop the virtualbox from running the OS.

More to be follow...
