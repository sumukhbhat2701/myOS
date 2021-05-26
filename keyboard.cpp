#include "keyboard.h"

// It is defined in kernel.cpp, not in any other header file, so we declare it first
void print(char* s);

KeyboardDriver::KeyboardDriver(InterruptManager *manager)
: InterruptHandler(0x21, manager),
data_port(0x60),
command_port(0x64)
{
    // ignore the keys which were being pressed if any when OS was started and wait for it to be over
    while(command_port.read() & 0x1)
        data_port.read();

    // Get the current state of the keyboard modify it and set/write it back to our desired state:

    // command to notify the PIC to activate interrupts 
    command_port.write(0xAE);

    // command to get current state so that we can read from it in the next step from data port
    command_port.write(0x20);

    // read the current state from data port and change it to set next state in the next step here
    u8_t status = (data_port.read() | 1) & ~0x10;

    // command to set state
    command_port.write(0x60);

    // set the next state using data port
    data_port.write(status);

    // Finally fully activate the keyboard
    data_port.write(0xF4);




}

KeyboardDriver::~KeyboardDriver() { }

u32_t KeyboardDriver::interrupt_handler(u32_t esp)
{
    u8_t key = data_port.read();
    // key>=0x80 are key release, which we don't need. We want only keypress interrupt to find which key is presses. Otherwise we get 2 values on the screen when only 1 key is pressed.
    // ignore keys printed like when numlock etc are on.
    if(key < 0x80 && key != 0xFA && key != 0x45 && key != 0xC5)
    {
        u8_t shift = 0;
        switch(key)
        {
            case 0x02: if(shift) print("!"); else print("1"); break;
            case 0x03: if(shift) print("@"); else print("2"); break;
            case 0x04: if(shift) print("#"); else print("3"); break;
            case 0x05: if(shift) print("$"); else print("4"); break;
            case 0x06: if(shift) print("%"); else print("5"); break;
            case 0x07: if(shift) print("^"); else print("6"); break;
            case 0x08: if(shift) print("&"); else print("7"); break;
            case 0x09: if(shift) print("*"); else print("8"); break;
            case 0x0A: if(shift) print("("); else print("9"); break;
            case 0x0B: if(shift) print(")"); else print("0"); break;

            case 0x10: if(shift) print("Q"); else print("q"); break;
            case 0x11: if(shift) print("W"); else print("w"); break;
            case 0x12: if(shift) print("E"); else print("e"); break;
            case 0x13: if(shift) print("R"); else print("r"); break;
            case 0x14: if(shift) print("T"); else print("t"); break;
            case 0x15: if(shift) print("Y"); else print("y"); break;
            case 0x16: if(shift) print("U"); else print("u"); break;
            case 0x17: if(shift) print("I"); else print("i"); break;
            case 0x18: if(shift) print("O"); else print("o"); break;
            case 0x19: if(shift) print("P"); else print("p"); break;

            case 0x1E: if(shift) print("A"); else print("a"); break;
            case 0x1F: if(shift) print("S"); else print("s"); break;
            case 0x20: if(shift) print("D"); else print("d"); break;
            case 0x21: if(shift) print("F"); else print("f"); break;
            case 0x22: if(shift) print("G"); else print("g"); break;
            case 0x23: if(shift) print("H"); else print("h"); break;
            case 0x24: if(shift) print("J"); else print("j"); break;
            case 0x25: if(shift) print("K"); else print("k"); break;
            case 0x26: if(shift) print("L"); else print("l"); break;

            case 0x2C: if(shift) print("Z"); else print("z"); break;
            case 0x2D: if(shift) print("X"); else print("x"); break;
            case 0x2E: if(shift) print("C"); else print("c"); break;
            case 0x2F: if(shift) print("V"); else print("v"); break;
            case 0x30: if(shift) print("B"); else print("b"); break;
            case 0x31: if(shift) print("N"); else print("n"); break;
            case 0x32: if(shift) print("M"); else print("m"); break;
            case 0x33: if(shift) print("<"); else print(","); break;
            case 0x34: if(shift) print(">"); else print("."); break;
            case 0x35: if(shift) print("_"); else print("-"); break;

            case 0x52: if(!shift) print("0"); break;
            case 0x4F: if(!shift) print("1"); break;
            case 0x51: if(!shift) print("3"); break;
            case 0x4B: if(!shift) print("4"); break;
            case 0x4C: if(!shift) print("5"); break;
            case 0x4D: if(!shift) print("6"); break;
            case 0x47: if(!shift) print("7"); break;
            case 0x48: if(!shift) print("8"); break;
            case 0x49: if(!shift) print("9"); break;
            case 0x50: if(!shift) print("2"); break;

            case 0x1C: print("\n"); break;
            case 0x39: print(" "); break;
            // case 
            // case 0x33: if(shift) print("") 

            // left and right shift key press interrupt
            case 0x2A: case 0x36: shift = 1; break;
            // left and right shift key release interrupt
            case 0xAA: case 0xB6: shift = 0; break; 

            default:
            {
                char *message = "0x00 ";
                char *hexa = "0123456789ABCDEF";
                message[2] = hexa[(key >> 4) & 0x0F];
                message[3] = hexa[key & 0x0F];
                print(message);
            }
        }

        // to print characters instead of interrupt_id, we need to experiment each keys and map interrupt_id to their respective key value using a if-else or switch :"(
    }
    
    return esp;

}