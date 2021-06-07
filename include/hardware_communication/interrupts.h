#ifndef INTERRUPTS_H
#define INTERRUPTS_H 
#include <hardware_communication/port.h>
#include <gdt.h>
#include <multitasking.h>

namespace myOS
{
    namespace hardware_communication
    {
        class InterruptHandler;

        class InterruptManager
        {
            // to access its protected member for *handlers[256]
            friend class InterruptHandler;
            protected:
                // static pointer to be accessible by static members(like interrupt_handler()) of the class as well(because it cannot be done by objects)
                // points to which interrupt is active
                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];
                TaskManager* task_manager;

                struct GateDescriptor
                {
                    myOS::common::u16_t handler_address_low_bits;
                    myOS::common::u16_t gdt_code_segment_selector;
                    myOS::common::u8_t reserved;
                    myOS::common::u8_t access;
                    myOS::common::u16_t handler_address_high_bits;
                } __attribute__((packed));

                static GateDescriptor InterruptDescriptorTable[256];

                struct interruptDescriptorTablePointer
                {
                    myOS::common::u16_t size;
                    myOS::common::u32_t base;
                }__attribute__((packed));
                

                static void set_interrupt_descriptor_table_entry(myOS::common::u8_t interrupt_id, myOS::common::u16_t code_segment_selector_offset, void (* interrupt_handler)(), myOS::common::u8_t descriptor_previlige_level,myOS::common::u8_t descriptor_type);

                Port_8bits_slow pic_master_command;
                Port_8bits_slow pic_master_data;
                Port_8bits_slow pic_slave_command;
                Port_8bits_slow pic_slave_data;

            public:

                InterruptManager(GlobalDescriptorTable* gdt, TaskManager* task_manager);
                ~InterruptManager();

                // activate or deactivate an interrupt
                void activate();
                void deactivate();

                // static functions need no objects to access them and hence can be called in interruptstub.s as well easily
                static myOS::common::u32_t interrupt_handler(myOS::common::u8_t interrupt_id,myOS::common::u32_t esp);
                // non-static/member function analogous to interrupt_handler() which is called in it which is accessed by ActiveInterruptManager pointer member of the class(as static function can't be accessed by memebers of the class as they technically are not the functions of the class)
                myOS::common::u32_t interrupt_handler_main(myOS::common::u8_t interrupt_id, myOS::common::u32_t esp);

                // ignore the interrupts from devices whose routines are not configured
                static void ignore_interrupt_request();
                // timer interrupt
                static void handle_interrupt_request_0x00();
                // keyboard interrupt
                static void handle_interrupt_request_0x01();
                // mouse interrupt
                static void handle_interrupt_request_0x0C();

                // timer exception
                static void handle_exception_0x00();
                // keyboard exception
                static void handle_exception_0x01();
                // mouse exception
                static void handle_exception_0x0C();
        };

        class InterruptHandler
        {
            protected:
                myOS::common::u8_t interrupt_id;
                InterruptManager* interruptManager;

                InterruptHandler(myOS::common::u8_t interrupt_id, InterruptManager* interruptManager);
                ~InterruptHandler();

            public:
                // Why virtual?
                // MAYBE: virtual because we need to override this function, this case friend class
                virtual myOS::common::u32_t interrupt_handler(myOS::common::u32_t esp);
        };

    }
}

#endif

/*
NOTE:
- Static methods can be invoked without objects of a class.
- Static methods of the class can't be invoked using objects of the class.
- Static+member+self referential pointers can access both static and non-static member functions using ->. 
- Static member function can only access static data member, other static member functions and any other functions from outside the class.
*/

// IDT entry:
// https://wiki.osdev.org/Interrupt_Descriptor_Table

// This is similar to the GDT, except:
// The first entry (at zero offset) is used in the IDT.
// There are 256 interrupts (0..255), so IDT should have 256 entries, each entry corresponding to a specific interrupt.
// It can contain more or less than 256 entries. More entries are ignored. When an interrupt or exception is invoked whose entry is not present, an exception 
// is raised that tells the number of the missing IDT entry, and even whether it was hardware or software interrupt. There should therefore be at least enough entries so an exception can be caught.