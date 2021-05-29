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

        class  PCIController
        {
            myOS::hardware_communication::Port_32bits data_port;
            myOS::hardware_communication::Port_32bits command_port;
            public:
                PCIController();
                ~PCIController();

                myOS::common::u32_t read(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function,myOS::common::u32_t register_offset);
                void write(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function,myOS::common::u32_t register_offset, myOS::common::u32_t data);
                myOS::common::u8_t device_has_functions(myOS::common::u16_t bus,myOS::common::u16_t device);

                void select_drivers(myOS::drivers::DriverManager* driverManager);
                PCIDeviceDescriptor get_device_descriptor(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function);
        };

    }
}

#endif

// each PCI 
// => 8 connectable buses
// => each bus => 32 connectable devices
// => each device => 8 functions, defined in certain memory/register(register_offset)
