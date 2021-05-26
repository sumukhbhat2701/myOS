#include "gdt.h"
#include "interrupts.h"
#include "types.h"
#include "keyboard.h"

// Write our own print function as we do not have IO header files in our new OS space.
void print(char* s)
{
	// every (second)alternate byte from memory address 0xb8000, when anything written into those bytes are displayed on the screen. Every other alternate memory bytes are used to specify color(4 bits for foreground and 4 bits for background color, default is set to white text on black background).
	// TYPE* ptr = (TYPE *)mem_address => ptr points to a mem location starting from mem_address and sizeof sizeof(TYPE)
	// As we need alternate blocks, we use unsigned short(u16_t) which is 2 bytes instead of 4 incase of unsigned int(u32_t). screen+i by 2*i bytes incase of u16_t, which is 4*i bytes incase of u32_t. 
	// As we need only 2nd byte of every 2 bytes of u16_t, we 'and' the mem location with 2 bytes with 0xFF00 and 'or' with 1 byte of character to be written.  
	
	// cursor location - (x,y)
	static u8_t x = 0, y = 0;
		
	static u16_t* screen = (u16_t *)0xb8000;
	for(int i = 0; s[i] != '\0'; i++)
	{
		if(s[i] == '\n')
		{
			y++;
			x = 0;
		}
		else
		{
			*(screen + (80*y + x)) = (*(screen + (80*y + x)) & 0xFF00) | s[i];
			x++;
		}

		if(x >= 80)
		{
			y++;
			x = 0;
		}

		if(y>=25)
		{
			for(y = 0; y<25; y++)
				for(x = 0; x< 80; x++)
					*(screen + (80*y + x)) = (*(screen + (80*y + x)) & 0xFF00) | ' ';
			x = y = 0;
		}
		
	}
}

// extern "C" makes a function-name in C++ have C compilation to form a obj(.o) file (compiler does not mangle the name). As kernel_main is accessed in loader.s as kernel_main itself and as g++(C++) compiler changes name of functions while linking, we need to make use of C compilation method for this function.

typedef void (*ctor)();
// All ctors of global objects are stored between start_ctor and end_ctor (defined in linker.ld's .data section)
extern "C" ctor start_ctor;
extern "C" ctor end_ctor;
// invoke all the ctors of global objects stored between start_ctor and end_ctor manually. 
extern "C" void call_ctors()
{
	for(ctor* c = &start_ctor;c != &end_ctor;c++)
	{
		(*c)();
	}
}

// functions paramters are from ax and bx registers pushed in loader.s
extern "C" void kernel_main(void* multiboot_structure, unsigned int magic_number)  
{
	print("Yay! You made it!\n");

	GlobalDescriptorTable gdt;
	InterruptManager interrupts(&gdt);

	KeyboardDriver keyboard(&interrupts);

	interrupts.activate();

	// infinite loop as kernel should be running at all times	
	while(1);
}
