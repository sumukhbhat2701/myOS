#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"

class MouseEventHandler;
// A driver, or device driver, is a set of files that tells a piece of hardware how to function by communicating with a computer's operating system. 
class MouseDriver : public InterruptHandler, public Driver
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

    MouseEventHandler* eventHandler;
    public:
        MouseDriver(InterruptManager *manager, MouseEventHandler* eventHandler);
        ~MouseDriver();
        virtual u32_t interrupt_handler(u32_t esp);
        virtual void activate();

};

// general event handler for mouse, whose functions are overriden for specific functionalities
class MouseEventHandler
{
    public:
        MouseEventHandler();
        ~MouseEventHandler();
        
        virtual void on_mouse_up(u8_t);
        virtual void on_mouse_down(u8_t);
        virtual void on_mouse_move(s32_t, s32_t);
};

// we define the behaviour of each event separately by deriving from MouseEventHandler base and overriding the functions to perform different operations on mouse movements
class DisplayMouseEventHandler : public MouseEventHandler
{
    // coordinates of the cursor
    s8_t x, y;
    public:
        DisplayMouseEventHandler();

        // overriden from base classs
        void on_mouse_move(s32_t xOffset, s32_t yOffset);

        // override on_mouse_up/down in the same way to get specific functionality
};

#endif