#ifndef GDT_H
#define GDT_H
#include <common/types.h>

namespace myOS
{
    class GlobalDescriptorTable
    {
        public:
            class SegmentDescriptor
            {
                private:
                    myOS::common::u16_t limit_lo;
                    myOS::common::u16_t base_lo;
                    myOS::common::u8_t base_hi;
                    myOS::common::u8_t type;
                    myOS::common::u8_t flags_limit_hi;
                    myOS::common::u8_t base_vhi;
                public:
                    SegmentDescriptor(myOS::common::u32_t base, myOS::common::u32_t limit, myOS::common::u8_t type);
                    myOS::common::u32_t compute_base();
                    myOS::common::u32_t compute_limit();

            } __attribute__((packed)); // inform the compiler to keep the values "byte-perfect" and not make changes interms of optimization or anything else

            GlobalDescriptorTable();
            ~GlobalDescriptorTable();
            myOS::common::u16_t compute_offset_codeSegmentSelector();
            myOS::common::u16_t compute_offset_dataSegmentSelector();
            
        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

    };

}

#endif

// main memory: 
// | kernel space | user space |

// user space:
// | code segment | data segment | ....other segments |

// each entry for global descriptor table:
// base of a segment: points to starting address of the segment, limit of a segment: length of the segment, flags = which segment - code/data/unused etc, access rights(kernel/user)  
// https://wiki.osdev.org/Global_Descriptor_Table

// | base(1 byte) | flags(1/2 byte) | limit(1/2 byte) | Access rights(1 byte) | base(3 bytes) | limit(2 bytes) |  
// 8 bytes   <----------------------------------------------------------------------------------------------   0
//    entry[7]       entry[6]          entry[6]              entry[5]           entry[4,3,2]      entry[1,0]