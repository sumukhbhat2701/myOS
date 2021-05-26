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
        switch(key)
        {
            case 0x02: print("1"); break;
            case 0x03: print("2"); break;
            case 0x04: print("3"); break;
            case 0x05: print("4"); break;
            case 0x06: print("5"); break;
            case 0x07: print("6"); break;
            case 0x08: print("7"); break;
            case 0x09: print("8"); break;
            case 0x0A: print("9"); break;
            case 0x0B: print("0"); break;

            case 0x10: print("q"); break;
            case 0x11: print("w"); break;
            case 0x12: print("e"); break;
            case 0x13: print("r"); break;
            case 0x14: print("t"); break;
            case 0x15: print("y"); break;
            case 0x16: print("u"); break;
            case 0x17: print("i"); break;
            case 0x18: print("o"); break;
            case 0x19: print("p"); break;

            case 0x1E: print("a"); break;
            case 0x1F: print("s"); break;
            case 0x20: print("d"); break;
            case 0x21: print("f"); break;
            case 0x22: print("g"); break;
            case 0x23: print("h"); break;
            case 0x24: print("j"); break;
            case 0x25: print("k"); break;
            case 0x26: print("l"); break;

            case 0x2C: print("z"); break;
            case 0x2D: print("x"); break;
            case 0x2E: print("c"); break;
            case 0x2F: print("v"); break;
            case 0x30: print("b"); break;
            case 0x31: print("n"); break;
            case 0x32: print("m"); break;
            case 0x33: print(","); break;
            case 0x34: print("."); break;
            case 0x35: print("-"); break;

            case 0x52: print("0"); break;
            case 0x4F: print("1"); break;
            case 0x51: print("3"); break;
            case 0x4B: print("4"); break;
            case 0x4C: print("5"); break;
            case 0x4D: print("6"); break;
            case 0x47: print("7"); break;
            case 0x48: print("8"); break;
            case 0x49: print("9"); break;
            case 0x50: print("2"); break;

            case 0x1C: print("\n"); break;
            case 0x39: print(" "); break;

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