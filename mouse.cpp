#include "mouse.h"

// It is defined in kernel.cpp, not in any other header file, so we declare it first
void print(char* s);

MouseDriver::MouseDriver(InterruptManager *manager)
: InterruptHandler(0x2C, manager),  //0x2C is interrupt number for mouse 
data_port(0x60),
command_port(0x64)
{
    // Make the mouse cursor visible at center of screen(40,12) initially.
    s8_t x = 40 ,y = 12;
    static u16_t* screen = (u16_t *)0xb8000;
    screen[80*y + x] = ((screen[80*y+x] & 0xF000) >> 4)| ((screen[80*y+x] & 0x0F00) << 4) | (screen[80*y+x] & 0x00FF);

    offset = buttons = 0;

    // Get the current state of the Mouse modify it and set/write it back to our desired state:

    // command to notify the PIC to activate interrupts 
    command_port.write(0xA8);

    // command to get current state so that we can read from it in the next step from data port
    command_port.write(0x20);

    // read the current state from data port and change it to set next state in the next step here
    // set the 2nd bit to 1
    u8_t status = (data_port.read() | 0x2);

    // command to set state
    command_port.write(0x60);

    // set the next state using data port
    data_port.write(status);

    // Finally fully activate the Mouse
    data_port.write(0xF4);

    command_port.write(0xD4);
    data_port.write(0xF4);
    data_port.read();
}

MouseDriver::~MouseDriver() { }

u32_t MouseDriver::interrupt_handler(u32_t esp)
{
    u8_t status = command_port.read();
    // only is the 6th bit of status is set, then we data to read, otherwise no mouse activity made
    if(!(status & 0x20))
    {
        return esp;
    }

    // buffer[1] => movement in +ve x-axis, buffer[2] => movement  in -ve y-axis
    // x and y act like mouse pointer/cursor poistion
    // initially at center of screen (40,12)
    static s8_t x = 40 ,y = 12;
    buffer[offset] = data_port.read();
    offset = (offset + 1)%3;

    // when offset returns back to 0, transition of buffer array is done and we can extract data from it as some mouse activity was made
    if(offset == 0)
    {
        static u16_t* screen = (u16_t *)0xb8000;

        // swap the contents of low 4 bits and 4 high low bits of alternate bytes in video memory(color part) to make the old position of cursor invisible(black box)
        screen[80*y + x] = ((screen[80*y+x] & 0xF000) >> 4)| ((screen[80*y+x] & 0x0F00) << 4) | (screen[80*y+x] & 0x00FF);

        x+=buffer[1];
        // avoid pointer to go out of screen boundries
        if(x < 0) x = 0 ;
        if(x >= 80) x = 79;

        y-=buffer[2];
        // avoid pointer to go out of screen boundries
        if(y < 0) y = 0;
        if(y >= 25) y = 24;

        // swap the contents of low 4 bits and 4 high low bits of alternate bytes in video memory(color part) to make the new position of cursor visible(white box)
        screen[80*y + x] = ((screen[80*y+x] & 0xF000) >> 4)| ((screen[80*y+x] & 0x0F00) << 4) | (screen[80*y+x] & 0x00FF);

        // button press - buffer[0]
        // if ith(i = 0 to 2) bit of buffer[0] changes from old values of buffer[0] i.e. button variable, then the button represented by ith bit was pressed
        for(u8_t i = 0; i < 3; i++)
        {
            // check every bit of buffer[0] and button
            if((buffer[0] & (1 << i)) != (buttons & (1<<i)))
            {
                //functionality of button represented by ith bytes
                
                // to recognize some change on button press, we have made the same changes as shown on cursor move
                screen[80*y + x] = ((screen[80*y+x] & 0xF000) >> 4)| ((screen[80*y+x] & 0x0F00) << 4) | (screen[80*y+x] & 0x00FF);
            }
        }
        // store the value of buffer[0] so that it can be compared with buffer[0] in next interrupt call to check if button was pressed
        buttons = buffer[0];
    
    }
   
    
    return esp;

}