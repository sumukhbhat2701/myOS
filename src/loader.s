# For the bootloader to identify any file loaded onto memory as kernel, the file should have the following values stored in the location
.set MAGIC, 0x1BADB002
.set FLAGS, (1<<0 | 1<<1)
# such that CHECKSUM+FLAGS+MAGIC = 0 when checked for errors
.set CHECKSUM, -(MAGIC + FLAGS)   
.section .multiboot
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	
.section .text

# functions defined in kernel:
.extern kernel_main
.extern call_ctors

# Entry point of main assembly code
.global loader
loader:
	# Eventhough instruction pointer(%ip) is suitably set to the right position by bootloader loaded onto memory, stack pointer(%esp) is not set by it. But cpp programs expect the stack pointer to be set to a stack before executing its code. So we manually set the stack pointer to a stack.
	mov $kernel_stack, %esp
	
	// invoking all the ctors of global objects stored between start_ctor and end_ctor defined in kernel.cpp manually. 
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
	
	
# define the kernel_stack to which stack pointer is set	
.section .bss
# As stack grows from right to left and kernel is loaded left to right, set the stack pointer leaving an offset of 2 megabytes for loaded kernel to grow(assuming size <= 2MiB) so that it doesn't overwrite things that are already loaded like bootloader etc
.space 2*1024*1024 
kernel_stack:
	

