/* Write our linker file as loader.o and kernel.o are generated by compilers by 2 different languages. So we manually link them to get kernel.bin */

ENTRY(loader)
/* "objdump -x <any_file>.o" to find-out what is the output file format. */
OUTPUT_FORMAT(elf32-i386)
/* "objdump -i" to find-out the architecture supported by the system */
OUTPUT_ARCH(i386:i386)

SECTIONS
{
	. = 0x0100000;
	
	/* combining all .text sections */
	.text :
	{
		/* .multiboot section of loader.s */
		*(.multiboot)
		/* .text sections of all other .o files */
		*(.text*)
		*(.rodata)
	}
	
	/* combining all .data sections */
	.data :
	{
		/* manual calls to the constructors of global objects */
		start_ctor = .;
		KEEP(*( ..init_array ));
		KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
		end_ctor = .;
		
		/* .data sections of all other .o files */
		*(.data)
	}
	
	/* combining all .bss sections */
	.bss :
	{
		*(.bss)
	}
	
	/* specify to ignore and discard anything other part of code left like comments. */
	/DISCARD/ :
	{
		*(.fini_array*)
		*(.comment)
	}
	
}

