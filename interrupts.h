#ifndef INTERRUPTS_H
#define INTERRUPTS_H 
#include "port.h"
#include "gdt.h"

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

        struct GateDescriptor
        {
            u16_t handler_address_low_bits;
            u16_t gdt_code_segment_selector;
            u8_t reserved;
            u8_t access;
            u16_t handler_address_high_bits;
        } __attribute__((packed));

        static GateDescriptor InterruptDescriptorTable[256];

        struct interruptDescriptorTablePointer
        {
            u16_t size;
            u32_t base;
        }__attribute__((packed));
        

        static void set_interrupt_descriptor_table_entry(u8_t interrupt_id, u16_t code_segment_selector_offset, void (* interrupt_handler)(), u8_t descriptor_previlige_level, u8_t descriptor_type);

        Port_8bits_slow pic_master_command;
        Port_8bits_slow pic_master_data;
        Port_8bits_slow pic_slave_command;
        Port_8bits_slow pic_slave_data;

    public:

        InterruptManager(GlobalDescriptorTable* gdt);
        ~InterruptManager();

        // activate or deactivate an interrupt
        void activate();
        void deactivate();

        // static functions need no objects to access them and hence can be called in interruptstub.s as well easily
        static u32_t interrupt_handler(u8_t interrupt_id, u32_t esp);
        // non-static/member function analogous to interrupt_handler() which is called in it which is accessed by ActiveInterruptManager pointer member of the class(as static function can't be accessed by memebers of the class as they technically are not the functions of the class)
        u32_t interrupt_handler_main(u8_t interrupt_id, u32_t esp);

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
        u8_t interrupt_id;
        InterruptManager* interruptManager;

        InterruptHandler(u8_t interrupt_id, InterruptManager* interruptManager);
        ~InterruptHandler();

    public:
        // Why virtual?
        // MAYBE: virtual because we need to override this function, this case friend class
        virtual u32_t interrupt_handler(u32_t esp);
};


#endif

/*
NOTE:
- Static methods can be invoked without objects of a class.
- Static methods of the class can't be invoked using objects of the class.
- Static+member+self referential pointers can access both static and non-static member functions using ->. 
- Static member function can only access static data member, other static member functions and any other functions from outside the class.
*/