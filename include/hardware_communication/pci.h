#ifndef PCI_H
#define PCI_H
#include<common/types.h>
#include<hardware_communication/port.h>
#include<hardware_communication/interrupts.h>
#include<drivers/driver.h>

// instead of explicilty invoking drivers, make use of pci and make it directly connect to the drivers

namespace myOS
{
    namespace hardware_communication
    {
        class BaseAddressRegister;

        class PCIDeviceDescriptor
        {
            public:
                myOS::common::u32_t port_base;
                myOS::common::u32_t interrupt;

                myOS::common::u16_t bus;
                myOS::common::u16_t device;
                myOS::common::u16_t function;
                
                myOS::common::u16_t vendor_id;
                myOS::common::u16_t device_id;
                
                myOS::common::u8_t class_id;
                myOS::common::u8_t subclass_id;
                myOS::common::u8_t interface_id;
                
                myOS::common::u8_t revision;

                PCIDeviceDescriptor();
                ~PCIDeviceDescriptor();

        };

        class PCIController
        {
            myOS::hardware_communication::Port_32bits data_port;
            myOS::hardware_communication::Port_32bits command_port;
            public:
                PCIController();
                ~PCIController();

                myOS::common::u32_t read(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function,myOS::common::u32_t register_offset);
                void write(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function,myOS::common::u32_t register_offset, myOS::common::u32_t data);
                myOS::common::u8_t device_has_functions(myOS::common::u16_t bus,myOS::common::u16_t device);

                void select_drivers(myOS::drivers::DriverManager* driverManager, myOS::hardware_communication::InterruptManager *interruptManager);
                PCIDeviceDescriptor get_device_descriptor(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function);
                BaseAddressRegister get_base_address_register(myOS::common::u16_t bus, myOS::common::u16_t device, myOS::common::u16_t function, myOS::common::u16_t bar);
                myOS::drivers::Driver* get_driver(myOS::hardware_communication::PCIDeviceDescriptor device, myOS::hardware_communication::InterruptManager* interruptManager);
        };

        // we use IO type of base register only and not memory mapped for this code
        enum BaseAddressRegisterType
        {
            memory_mapping = 0,
            io = 1
        };

        class BaseAddressRegister
        {
            public:
                myOS::common::u8_t prefetchable;
                myOS::common::u8_t* address;
                myOS::common::u32_t size;
                BaseAddressRegisterType type;
        };

    }
}

#endif

// each PCI 
// => 8 connectable buses
// => each bus => 32 connectable devices
// => each device => 8 functions, defined in certain memory/register(register_offset)


// Base Address Register(BAR):
// The base address registers are used to assign addresses to the resources of the PCI device. 

// 2 types:
// 1. IO: each byte is written and read simultaneously to and from the ports
// | 1 byte | 1 byte  |  1 byte | 6 bits  | 1 bit  | 1 bit |
// |<--------- port number -------------->|reserved| type=0|

// 2. Memory mapped: Data in bytes are written whenever produced to a memory location and read only when required from it.
// | 1 byte | 1 byte | 1 byte | 4 bits | 1 bit | 2 bits | 1 bit |
// |<--memory for data to be written-->|   x   |  ab    | type=1|
// x = 1 if device is prefetcable else 0
// ab = 00 if 32 bit data, 01 if 20 bit data, 10 if 64 bit data.