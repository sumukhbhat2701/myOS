#include <drivers/keyboard.h>

using namespace myOS::common;
using namespace myOS::drivers;
using namespace myOS::hardware_communication;

// It is defined in kernel.cpp, not in any other header file, so we declare it first
void print(char* s);
void print_hex(u32_t key);

KeyboardDriver::KeyboardDriver(InterruptManager *manager, KeyboardEventHandler *handler)
: InterruptHandler(0x21, manager),
data_port(0x60),
command_port(0x64)
{
    this->eventHandler = handler;
}

KeyboardDriver::~KeyboardDriver() { }

void KeyboardDriver::activate()
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

u32_t KeyboardDriver::interrupt_handler(u32_t esp)
{
    u8_t key = data_port.read();

    // if we donot have a event handler, we do not do anything
    if(eventHandler == 0)
    {
        return esp;
    }

    // key>=0x80 are key release, which we don't need(except 0xAA and 0xB6 which are used to make characters lower case once key release). We want only keypress interrupt to find which key is presses. Otherwise we get 2 values on the screen when only 1 key is pressed.
    // ignore keys printed like when numlock etc are on.
    if((key < 0x80 || key == 0xAA || key == 0xB6) && key != 0xFA && key != 0x45 && key != 0xC5)
    {
        // static because static variable have lifetime of whole program run, rather than once the functions scope ends as for every key pressed/released this function is invoked and the previous value of static wouldn't be remembered if non-static.
        static u8_t shift = 0;
        switch(key)
        {
            case 0x02: if(shift) eventHandler->on_key_down('!');else eventHandler->on_key_down('1');break;
            case 0x03: if(shift) eventHandler->on_key_down('@');else eventHandler->on_key_down('2');break;
            case 0x04: if(shift) eventHandler->on_key_down('#');else eventHandler->on_key_down('3');break;
            case 0x05: if(shift) eventHandler->on_key_down('$');else eventHandler->on_key_down('4');break;
            case 0x06: if(shift) eventHandler->on_key_down('%');else eventHandler->on_key_down('5');break;
            case 0x07: if(shift) eventHandler->on_key_down('^');else eventHandler->on_key_down('6');break;
            case 0x08: if(shift) eventHandler->on_key_down('&');else eventHandler->on_key_down('7');break;
            case 0x09: if(shift) eventHandler->on_key_down('*');else eventHandler->on_key_down('8');break;
            case 0x0A: if(shift) eventHandler->on_key_down('(');else eventHandler->on_key_down('9');break;
            case 0x0B: if(shift) eventHandler->on_key_down(')');else eventHandler->on_key_down('0');break;

            case 0x10: if(shift) eventHandler->on_key_down('Q');else eventHandler->on_key_down('q');break;
            case 0x11: if(shift) eventHandler->on_key_down('W');else eventHandler->on_key_down('w');break;
            case 0x12: if(shift) eventHandler->on_key_down('E');else eventHandler->on_key_down('e');break;
            case 0x13: if(shift) eventHandler->on_key_down('R');else eventHandler->on_key_down('r');break;
            case 0x14: if(shift) eventHandler->on_key_down('T');else eventHandler->on_key_down('t');break;
            case 0x15: if(shift) eventHandler->on_key_down('Y');else eventHandler->on_key_down('y');break;
            case 0x16: if(shift) eventHandler->on_key_down('U');else eventHandler->on_key_down('u');break;
            case 0x17: if(shift) eventHandler->on_key_down('I');else eventHandler->on_key_down('i');break;
            case 0x18: if(shift) eventHandler->on_key_down('O');else eventHandler->on_key_down('o');break;
            case 0x19: if(shift) eventHandler->on_key_down('P');else eventHandler->on_key_down('p');break;

            case 0x1E: if(shift) eventHandler->on_key_down('A');else eventHandler->on_key_down('a');break;
            case 0x1F: if(shift) eventHandler->on_key_down('S');else eventHandler->on_key_down('s');break;
            case 0x20: if(shift) eventHandler->on_key_down('D');else eventHandler->on_key_down('d');break;
            case 0x21: if(shift) eventHandler->on_key_down('F');else eventHandler->on_key_down('f');break;
            case 0x22: if(shift) eventHandler->on_key_down('G');else eventHandler->on_key_down('g');break;
            case 0x23: if(shift) eventHandler->on_key_down('H');else eventHandler->on_key_down('h');break;
            case 0x24: if(shift) eventHandler->on_key_down('J');else eventHandler->on_key_down('j');break;
            case 0x25: if(shift) eventHandler->on_key_down('K');else eventHandler->on_key_down('k');break;
            case 0x26: if(shift) eventHandler->on_key_down('L');else eventHandler->on_key_down('l');break;

            case 0x2C: if(shift) eventHandler->on_key_down('Z');else eventHandler->on_key_down('z');break;
            case 0x2D: if(shift) eventHandler->on_key_down('X');else eventHandler->on_key_down('x');break;
            case 0x2E: if(shift) eventHandler->on_key_down('C');else eventHandler->on_key_down('c');break;
            case 0x2F: if(shift) eventHandler->on_key_down('V');else eventHandler->on_key_down('v');break;
            case 0x30: if(shift) eventHandler->on_key_down('B');else eventHandler->on_key_down('b');break;
            case 0x31: if(shift) eventHandler->on_key_down('N');else eventHandler->on_key_down('n');break;
            case 0x32: if(shift) eventHandler->on_key_down('M');else eventHandler->on_key_down('m');break;
            case 0x33: if(shift) eventHandler->on_key_down('<');else eventHandler->on_key_down(',');break;
            case 0x34: if(shift) eventHandler->on_key_down('>');else eventHandler->on_key_down('.');break;
            case 0x35: if(shift) eventHandler->on_key_down('_');else eventHandler->on_key_down('-');break;

            case 0x52: if(!shift) eventHandler->on_key_down('0');break;
            case 0x4F: if(!shift) eventHandler->on_key_down('1');break;
            case 0x51: if(!shift) eventHandler->on_key_down('3');break;
            case 0x4B: if(!shift) eventHandler->on_key_down('4');break;
            case 0x4C: if(!shift) eventHandler->on_key_down('5');break;
            case 0x4D: if(!shift) eventHandler->on_key_down('6');break;
            case 0x47: if(!shift) eventHandler->on_key_down('7');break;
            case 0x48: if(!shift) eventHandler->on_key_down('8');break;
            case 0x49: if(!shift) eventHandler->on_key_down('9');break;
            case 0x50: if(!shift) eventHandler->on_key_down('2');break;

            case 0x1C: eventHandler->on_key_down('\n');break;
            case 0x39: eventHandler->on_key_down(' ');break;

            // left and right shift key press interrupt
            case 0x2A: case 0x36: shift = 1; break;
            // left and right shift key release interrupt
            case 0xAA: case 0xB6: shift = 0; break; 

            default:
            {
                print("Keyboard => ");
                print_hex(key);
            }
        }

        // to print characters instead of interrupt_id, we need to experiment each keys and map interrupt_id to their respective key value using a if-else or switch :"(
    }
    
    return esp;

}

KeyboardEventHandler::KeyboardEventHandler()
{
    
}

KeyboardEventHandler::~KeyboardEventHandler()
{

}

void KeyboardEventHandler::on_key_down(char)
{

}

void KeyboardEventHandler::on_key_up(char)
{

}

void PrintKeyboardEventHandler::on_key_down(char ch)
{
    char* message = " ";
    message[0] = ch;
    print(message);
}