#ifndef GDT_H
#define GDT_H
#include "types.h"

class GlobalDescriptorTable
{
    private:
        class SegmentDescriptor
        {
            private:
                u16_t limit_lo;
                u16_t base_lo;
                u8_t base_hi;
                u8_t type;
                u8_t flags_limit_hi;
                u8_t base_vhi;
            public:
                SegmentDescriptor(u32_t base, u32_t limit, u8_t type);
                u32_t compute_base();
                u32_t compute_limit();

        } __attribute__((packed)); // inform the compiler to keep the values "byte-perfect" and not make changes interms of optimization or anything else
        SegmentDescriptor nullSegmentSelector;
        SegmentDescriptor unusedSegmentSelector;
        SegmentDescriptor codeSegmentSelector;
        SegmentDescriptor dataSegmentSelector;
    
    public:
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();
        u16_t compute_offset_codeSegmentSelector();
        u16_t compute_offset_dataSegmentSelector();

};


#endif