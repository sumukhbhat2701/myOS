#ifndef PORT_H
#define PORT_H
#include <common/types.h>

namespace myOS
{
    namespace hardware_communication
    {
        class Port
        {
            protected:
                myOS::common::u16_t port_number;
                Port(myOS::common::u16_t port_number);
                ~Port();
        };

        class Port_8bits : public Port
        {
            public:
                Port_8bits(myOS::common::u16_t port_number);
                ~Port_8bits();
                virtual void write(myOS::common::u8_t data);
                virtual myOS::common::u8_t read();
        };

        class Port_8bits_slow : public Port_8bits
        {
            public:
                Port_8bits_slow(myOS::common::u16_t port_number);
                ~Port_8bits_slow();
                virtual void write(myOS::common::u8_t data);
                // inherit read(), non need to overwrite
        };

        class Port_16bits : public Port
        {
            public:
                Port_16bits(myOS::common::u16_t port_number);
                ~Port_16bits();
                virtual void write(myOS::common::u16_t data);
                virtual myOS::common::u16_t read();
        };

        class Port_32bits : public Port
        {
            public:
                Port_32bits(myOS::common::u16_t port_number);
                ~Port_32bits();
                virtual void write(myOS::common::u32_t data);
                virtual myOS::common::u32_t read();
        };


    }
}

#endif