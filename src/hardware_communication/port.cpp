#include <hardware_communication/port.h>

using namespace myOS::common;
using namespace myOS::hardware_communication;

Port::Port(u16_t port_number)
{
    this->port_number = port_number;
}

Port::~Port() {}

Port_8bits::Port_8bits(u16_t port_number)
:Port(port_number)
{

}

Port_8bits::~Port_8bits() {}

void Port_8bits::write(u8_t data)
{
    __asm__ volatile("outb %0, %1": : "a"(data), "Nd"(port_number));
}

u8_t Port_8bits::read()
{
    u8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}


Port_16bits::Port_16bits(u16_t port_number)
:Port(port_number)
{

}

Port_16bits::~Port_16bits() {}

void Port_16bits::write(u16_t data)
{
    __asm__ volatile("outw %0, %1": : "a"(data), "Nd"(port_number));
}

u16_t Port_16bits::read()
{
    u16_t result;
    __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}


Port_32bits::Port_32bits(u16_t port_number)
:Port(port_number)
{

}

Port_32bits::~Port_32bits() {}

void Port_32bits::write(u32_t data)
{
    __asm__ volatile("outl %0, %1": : "a"(data), "Nd"(port_number));
}

u32_t Port_32bits::read()
{
    u32_t result;
    __asm__ volatile("inl %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}


Port_8bits_slow::Port_8bits_slow(u16_t port_number)
:Port_8bits(port_number)
{

}

Port_8bits_slow::~Port_8bits_slow() {}

// stall the execution for a moment using garbage instruction until the port is done writing data 
void Port_8bits_slow::write(u8_t data)
{
    __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (this->port_number));
}


