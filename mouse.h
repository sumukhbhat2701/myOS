#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

// A driver, or device driver, is a set of files that tells a piece of hardware how to function by communicating with a computer's operating system. 
class MouseDriver : public InterruptHandler
{
    Port_8bits data_port;
    Port_8bits command_port;

    // buffer[1] => movement in +ve x-axis, buffer[2] => movement  in -ve y-axis
    // button press - buffer[0]
    u8_t buffer[3];
    //index of buffer array
    u8_t offset;
    // old value of buffer[0]
    // if ith bit of buffer[0] changes from old values of buffer[0] i.e. button variable, then the button represented by ith bit was pressed
    u8_t buttons;
    public:
        MouseDriver(InterruptManager *manager);
        ~MouseDriver();
        virtual u32_t interrupt_handler(u32_t esp);

};

#endif