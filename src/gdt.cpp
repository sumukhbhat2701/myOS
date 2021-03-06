#include <gdt.h>

using namespace myOS::common;
using namespace myOS;

GlobalDescriptorTable::GlobalDescriptorTable():
nullSegmentSelector(0,0,0),
unusedSegmentSelector(0,0,0),
codeSegmentSelector(0, 64*1024*1024, 0x9A), //64 megabytes
dataSegmentSelector(0, 64*1024*1024, 0x92)
{
    u32_t entry[2];
    entry[1] = (u32_t)this;
    entry[0] = sizeof(GlobalDescriptorTable) << 16; //16 high bytes

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
    u8_t* entry = (u8_t *)this;
    // encode base and limit
    if(limit <= 65536) 
    {
        // <= 2^16 (16 bit address space => 16 bit limit) => entry[6] = 0xC0
        entry[6] = 0x40;
    }
    else 
    {
        // 32 bit address space => entry[6] = 0xC0
        // Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
        // This is done by discarding the 12 least significant bits, but this
        // is only legal, if they are all ==1, so they are implicitly still there

        // so if the last bits aren't all 1, we have to set them to 1, but this
        // would increase the limit (cannot do that, because we might go beyond
        // the physical limit or get overlap with other segments) so we have to
        // compensate this by decreasing a higher bit (and might have up to
        // 4095 wasted bytes behind the used memory)
        if((limit & 0xFFF) != 0xFFF)
            limit = (limit>>12)-1;
        else    
            limit = limit >> 12;
        
        entry[6] = 0xC0;

    }

    entry[0] = limit & 0xFF;
    entry[1] = (limit >> 8) & 0xFF;
    entry[6] |= (limit >> 16) & 0xF;

    entry[2] = base & 0xFF;
    entry[3] = (base >> 8) & 0xFF;
    entry[4] = (base >> 16) & 0xFF;
    entry[7] = (base >> 24) & 0xFF;

    entry[5] = flags;

}

u32_t GlobalDescriptorTable::SegmentDescriptor::compute_base()
{
    // decode
    u8_t* target = (u8_t *)this;
    u32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

u32_t GlobalDescriptorTable::SegmentDescriptor::compute_limit()
{
    // decode
    u8_t* target = (u8_t *)this;
    u32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF;
    
    return result;
}

// GDT entry:
// | base(1 byte) | flags(1/2 byte) | limit(1/2 byte) | Access rights(1 byte) | base(3 bytes) | limit(2 bytes) |  
// 8 bytes   <----------------------------------------------------------------------------------------------   0
//    entry[7]       entry[6]          entry[6]              entry[5]           entry[4,3,2]      entry[1,0]
