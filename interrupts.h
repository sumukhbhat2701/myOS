#ifndef INTERRUPTS_H
#define INTERRUPTS_H 
#include "port.h"
#include "gdt.h"

class InterruptManager
{
    protected:
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

        void activate();

        // all the below defined in assembly interruptstubs.s and not in interrupts.cpp:

        static u32_t interrupt_handler(u8_t interrupt_id, u32_t esp);

        static void ignore_interrupt_request();
        // timer interrupt
        static void handle_interrupt_request_0x00();
        // keyboard interrupt
        static void handle_interrupt_request_0x01();

        // timer interrupt
        static void handle_exception_0x00();
        // keyboard interrupt
        static void handle_exception_0x01();

};

#endif