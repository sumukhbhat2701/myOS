#include <gdt.h>
#include <hardware_communication/interrupts.h>
#include <common/types.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>
#include <hardware_communication/pci.h>
#include <drivers/vga.h>
#include <gui/desktop.h>

using namespace myOS::common;
using namespace myOS::hardware_communication;
using namespace myOS::drivers;
using namespace myOS;
using namespace myOS::gui;

// Write our own print function as we do not have IO header files in our new OS space.
void print(char* s)
{
	// every (second)alternate byte from memory address 0xb8000, when anything written into those bytes are displayed on the screen. Every other alternate memory bytes are used to specify color(4 bits for foreground and 4 bits for background color, default is set to white text on black background).
	// TYPE* ptr = (TYPE *)mem_address => ptr points to a mem location starting from mem_address and sizeof sizeof(TYPE)
	// As we need alternate blocks, we use unsigned short(u16_t) which is 2 bytes instead of 4 incase of unsigned int(u32_t). screen+i by 2*i bytes incase of u16_t, which is 4*i bytes incase of u32_t. 
	// As we need only 2nd byte of every 2 bytes of u16_t, we 'and' the mem location with 2 bytes with 0xFF00 and 'or' with 1 byte of character to be written.  
	
	// static because static variable have lifetime of whole program run, rather than once the functions scope ends as for every key pressed/released this function is invoked and the previous value of static wouldn't be remembered if non-static.

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

void print_hex(u32_t key)
{
	char *message = "0x00";
	char *hexa = "0123456789ABCDEF";
	message[2] = hexa[(key >> 4) & 0x0F];
	message[3] = hexa[key & 0x0F];
	print(message);
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
	InterruptManager interruptManager(&gdt);

	print("Initializing hardware, Step1...");
	DriverManager driverManager;
	print("Done with Step1\n");
	
	PrintKeyboardEventHandler keyboardEventHandler;
	KeyboardDriver keyboard(&interruptManager, &keyboardEventHandler);
	driverManager.add_driver(&keyboard);
	
	DisplayMouseEventHandler mouseEventHandler;
	MouseDriver mouse(&interruptManager, &mouseEventHandler);
	driverManager.add_driver(&mouse);
	
	PCIController pciController;
	
	pciController.select_drivers(&driverManager, &interruptManager);

	VGA vga;

	print("Initializing hardware, Step2...");
	driverManager.activate_all();
	print("Done with Step2\n");

	print("Initializing hardware, Step3...");
	interruptManager.activate();
	print("Done with Step3\n");

	vga.set_mode(320, 200, 8);
	// vga.fill_rectangle(0, 0, 320, 200, 0, 0, 0xA8);

	Desktop desktop(320, 200, 0, 0, 0xA8);
	desktop.draw(&vga);


	// infinite loop as kernel should be running at all times	
	while(1);
}
