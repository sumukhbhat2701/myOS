#include "gdt.h"

GlobalDescriptorTable::GlobalDescriptorTable():
nullSegmentSelector(0,0,0),
unusedSegmentSelector(0,0,0),
codeSegmentSelector(0, 64*1024*1024, 0x9A), //64 megabytes
dataSegmentSelector(0, 64*1024*1024, 0x92)
{
    u32_t entry[2];
    entry[0] = (u32_t)this;
    entry[1] = sizeof(GlobalDescriptorTable) << 16; //16 high bytes

    // inform the processor to use ths table using lgdt => load global descriptor table 
    asm volatile("lgdt (%0)": :"p" (((u8_t *)entry)+2));
}

GlobalDescriptorTable::~GlobalDescriptorTable() { }

u16_t GlobalDescriptorTable::compute_offset_dataSegmentSelector()
{
    return (u8_t *)&dataSegmentSelector - (u8_t *)this;
}

u16_t GlobalDescriptorTable::compute_offset_codeSegmentSelector()
{
    return (u8_t *)&codeSegmentSelector - (u8_t *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(u32_t base, u32_t limit, u8_t flags)
{
    u8_t* target = (u8_t *)this;
    if(limit <= 65536)
    {
        target[6] = 0x40;
    }
    else
    {
        if((limit & 0xFFF) != 0xFFF)
            limit = (limit>>12)-1;
        else    
            limit = limit >> 12;
        
        target[6] = 0xC0;

    }

    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] = (limit >> 16) & 0xF;

    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;

    target[5] = flags;

}

u32_t GlobalDescriptorTable::SegmentDescriptor::compute_base()
{
    u8_t* target = (u8_t *)this;
    u32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

u32_t GlobalDescriptorTable::SegmentDescriptor::compute_limit()
{
    u8_t* target = (u8_t *)this;
    u32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF;
    
    return result;
}
