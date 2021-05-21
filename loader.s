# On boot, stack pointer(%esp) has to be manually set to kernel.bin loaded onto memory.

# For the bootloader to identify any file loaded onto memory as kernel, the file should have the following values stored in the location
.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	
.section .text

# functions defined in kernel:
.extern kernel_main
.extern call_ctors

.global loader
loader:
	mov $kernel_stack, %esp
	
	call call_ctors
	
	# bootloader loads a multiboot structure on memory pointed by ax register and magic number in bx register which acts like parameters in kerner_main() in kernel.cpp
	push %eax
	push %ebx
	call kernel_main

# infinite loop as kernel should be running at all times(just incase the inf loop in cpp file fails)
_stop:
	cli
	hlt
	jmp _stop
	
	
		
.section .bss
# As stack grows from right to left and kernel is loaded left to right, set the stack pointer leaving an offset of 2 megabytes for loaded kernel to grow(assuming size <= 2MiB) so that it doesn't overwrite things that are already loaded like bootloader etc
.space 2*1024*1024 
kernel_stack:
	

