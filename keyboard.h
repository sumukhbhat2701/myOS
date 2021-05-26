#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

// A driver, or device driver, is a set of files that tells a piece of hardware how to function by communicating with a computer's operating system. 
class KeyboardDriver : public InterruptHandler
{
    Port_8bits data_port;
    Port_8bits command_port;
    public:
        KeyboardDriver(InterruptManager *manager);
        ~KeyboardDriver();
        virtual u32_t interrupt_handler(u32_t esp);

};

#endif