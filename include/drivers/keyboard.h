#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <common/types.h>
#include <hardware_communication/interrupts.h>
#include <hardware_communication/port.h>
#include <drivers/driver.h>

namespace myOS
{
    namespace drivers
    {

        class KeyboardEventHandler;

        // A driver, or device driver, is a set of files that tells a piece of hardware how to function by communicating with a computer's operating system. 
        class KeyboardDriver : public myOS::hardware_communication::InterruptHandler, public Driver
        {
            myOS::hardware_communication::Port_8bits data_port;
            myOS::hardware_communication::Port_8bits command_port;
            KeyboardEventHandler *eventHandler;
            public:
                KeyboardDriver(myOS::hardware_communication::InterruptManager *manager, KeyboardEventHandler* eventHandler);
                ~KeyboardDriver();
                virtual myOS::common::u32_t interrupt_handler(myOS::common::u32_t esp);
                virtual void activate();

        };

        // general event handler for keyboard, whose functions are overriden for specific functionalities
        class KeyboardEventHandler
        {
            public:
                KeyboardEventHandler();
                ~KeyboardEventHandler();

                virtual void on_key_down(char);
                virtual void on_key_up(char);
        };

        // we define the behaviour of each event separately by deriving from KeyboardEventHandler base and overriding the functions to perform different operations on keypress and keyrelease
        // Eg: one for printing on the screen when a key pressed, and one derived class for performing different functionality when a key is pressed, like case change etc or gui change.
        class PrintKeyboardEventHandler : public KeyboardEventHandler
        {
            public:
                // overriden from base class
                void on_key_down(char ch);

        };
    }
}
#endif