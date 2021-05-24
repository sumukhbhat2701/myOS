#include "interrupts.h"

void print(char* s);

InterruptManager::GateDescriptor InterruptManager::InterruptDescriptorTable[256];

u32_t InterruptManager::interrupt_handler(u8_t interrupt_id, u32_t esp)
{
    print("INTERRUPTED");
    return esp;
}

void InterruptManager::set_interrupt_descriptor_table_entry(u8_t interrupt_id, u16_t code_segment_selector_offset, void (* interrupt_handler)(), u8_t descriptor_previlige_level, u8_t descriptor_type)
{
    const u8_t IDT_DESC_PRESENT = 0x80;

    InterruptDescriptorTable[interrupt_id].handler_address_low_bits = ((u32_t)interrupt_handler) & 0xFFFF;
    InterruptDescriptorTable[interrupt_id].handler_address_high_bits = ((u32_t)interrupt_handler >> 16) & 0xFFFF;
    InterruptDescriptorTable[interrupt_id].gdt_code_segment_selector = code_segment_selector_offset;
    InterruptDescriptorTable[interrupt_id].access = IDT_DESC_PRESENT | descriptor_type | ((descriptor_previlige_level & 0x3) << 5);
    InterruptDescriptorTable[interrupt_id].reserved = 0;
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
: pic_master_command(0x20),
pic_master_data(0x21),
pic_slave_command(0xA0),
pic_slave_data(0xA1)
{
    u16_t code_segment = gdt->compute_offset_codeSegmentSelector();
    const u8_t IDT_INTERRUPT_GATE = 0xE;

    // initialize all the IDT entries to interrupt_ignore()
    for(u16_t i = 0; i<256; i++)
    {
        set_interrupt_descriptor_table_entry(i, code_segment, &ignore_interrupt_request, 0, IDT_INTERRUPT_GATE);
    }

    // Update the entries we want to handle
    // 0x00 - timer ; pos = IRQ_BASE+0x00 = 0x20 + 0x00 = 0x20 ; IRQ_BASE is defined as 0x20 in interruptstub.s
    set_interrupt_descriptor_table_entry(0x20, code_segment, &handle_interrupt_request_0x00, 0, IDT_INTERRUPT_GATE);
    // 0x01 - timer ; pos = IRQ_BASE+0x01 = 0x21 + 0x00 = 0x20
    set_interrupt_descriptor_table_entry(0x21, code_segment, &handle_interrupt_request_0x01, 0, IDT_INTERRUPT_GATE);

    pic_master_command.write(0x11);
    pic_slave_command.write(0x11);

    pic_master_data.write(0x20);
    pic_slave_data.write(0x28);

    pic_master_data.write(0x04);
    pic_slave_data.write(0x02);

    pic_master_data.write(0x01);
    pic_slave_data.write(0x01);

    pic_master_data.write(0x00);
    pic_slave_data.write(0x00);

    interruptDescriptorTablePointer idt;
    idt.size = 256*sizeof(GateDescriptor) - 1;
    idt.base = (u32_t)InterruptDescriptorTable;
    // load interrupt descriptor table - lidt
    asm volatile("lidt %0" : : "m"(idt));
}

InterruptManager::~InterruptManager() {}

void InterruptManager::activate()
{
    // sti - start interrupts
    asm("sti");
}