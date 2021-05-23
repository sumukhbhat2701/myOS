#ifndef PORT_H
#define PORT_H
#include "types.h"

class Port
{
    protected:
        u16_t port_number;
        Port(u16_t port_number);
        ~Port();
};

class Port_8bits : public Port
{
    public:
        Port_8bits(u16_t port_number);
        ~Port_8bits();
        virtual void write(u8_t data);
        virtual u8_t read();
};

class Port_8bits_slow : public Port_8bits
{
    public:
        Port_8bits_slow(u16_t port_number);
        ~Port_8bits_slow();
        virtual void write(u8_t data);
        // inherit read(), non need to overwrite
};

class Port_16bits : public Port
{
    public:
        Port_16bits(u16_t port_number);
        ~Port_16bits();
        virtual void write(u16_t data);
        virtual u16_t read();
};

class Port_32bits : public Port
{
    public:
        Port_32bits(u16_t port_number);
        ~Port_32bits();
        virtual void write(u32_t data);
        virtual u32_t read();
};



#endif