#include<hardware_communication/pci.h>

using namespace myOS::common;
using namespace myOS::hardware_communication;
using namespace myOS::drivers;

void print(char *);
void print_hex(u32_t);

PCIController::PCIController()
:data_port(0xCFC),
command_port(0xCF8)
{

}

PCIController::~PCIController()
{
    
}

u32_t PCIController::read(u16_t bus_id,u16_t device_id,u16_t function_id,u32_t register_offset)
{
    // combined id with first bit set
    u32_t id = (0x1 << 31) | ((bus_id & 0xFF) << 16) | ((device_id & 0x1F) << 11) | ((function_id & 0x07) << 8) | ((register_offset) & 0xFC);
    command_port.write(id);
    u32_t result = data_port.read();
    // as u_32(int) is 4 bytes, 8 bits each but a device is represented by any bit, we must %4 and *8 to the register offset and get the corresponding bit in the result
    return result >> (8*(register_offset%4));
    

}

void PCIController::write(u16_t bus_id,u16_t device_id,u16_t function_id,u32_t register_offset, u32_t value)
{
    u32_t id = (0x1 << 31) | ((bus_id & 0xFF) << 16) | ((device_id & 0x1F) << 11) | ((function_id & 0x07) << 8) | ((register_offset) & 0xFC);
    command_port.write(id);
    data_port.write(value);
}

u8_t PCIController::device_has_functions(u16_t bus_id,u16_t device_id)
{
    return read(bus_id, device_id, 0, (0x0E)) & (1 << 7);
}

void PCIController::select_drivers(DriverManager* driverManager, InterruptManager *interruptManager)
{
    // list all the devices on the PCI which is managed by the driverManager
    // equivalent to "lspci" (additional flags: -n ,-x) and "lsusb" command on the linux/unix shells.

    for(u8_t bus = 0; bus < 8; bus++)
    {
        // vendor id => company's name + unique number for a hardware
        for(u8_t device = 0; device < 32; device++)
        {
            u8_t num_of_functions = PCIController::device_has_functions(bus, device) ? 8 : 1;
            for(u8_t function = 0; function < num_of_functions; function++)
            {
                PCIDeviceDescriptor device_ = get_device_descriptor(bus, device, function);

                if(device_.vendor_id == 0 || device_.vendor_id == 0xFFFF)
                {
                    continue;
                }
                

                // bar - base address register
                for(u8_t bar_num = 0; bar_num < 6; bar_num++)
                {
                    BaseAddressRegister bar = get_base_address_register(bus, device, function, bar_num);
                    // if address is set and it is of type io
                    if(bar.address && (bar.type == io))
                    {
                        // written as port_number(except 2 low bits) in BAR incase of IO and address(except 4 bits) in mem mapped
                        device_.port_base = (u32_t)bar.address;
                    }
                    Driver* driver = get_driver(device_, interruptManager);
                    // if we get a driver
                    if(driver != 0)
                    {
                        driverManager->add_driver(driver);
                    }
                }

                // display the details of all the devices which is occupying pci ports
                print("PCI BUS: ");
                print_hex(bus & 0xFF);

                print(", DEVICE: ");
                print_hex(device & 0xFF);

                print(", FUNCTION: ");
                print_hex(function & 0xFF);

                print(" => VENDOR LOW BYTES: ");
                print_hex((device_.vendor_id & 0xFF00) >> 8);
                print(" VENDOR HIGH BYTES: ");
                print_hex(device_.vendor_id & 0xFF);

                print(", DEVICE: ");
                print_hex((device_.device_id & 0xFF00) >> 8);
                print_hex(device_.device_id & 0xFF);
                print("; ");
            }
        }
    }
}

PCIDeviceDescriptor PCIController::get_device_descriptor(myOS::common::u16_t bus,myOS::common::u16_t device,myOS::common::u16_t function)
{
    PCIDeviceDescriptor res;
    res.bus = bus;
    res.device = device;
    res.function = function;
    res.vendor_id = read(bus, device, function, 0x00);
    res.device_id = read(bus, device, function, 0x02);
    res.class_id = read(bus, device, function, 0x0B);
    res.subclass_id = read(bus, device, function, 0x0A);
    res.interface_id = read(bus, device, function, 0x09);
    res.revision = read(bus, device, function, 0x08);
    res.interrupt = read(bus, device, function, 0x3C);

    return res;
}

PCIDeviceDescriptor::PCIDeviceDescriptor()
{

}

PCIDeviceDescriptor::~PCIDeviceDescriptor()
{

}

Driver* PCIController::get_driver(PCIDeviceDescriptor device, InterruptManager* interruptManager)
{
    Driver* driver = 0;

    // identify specific devices and their drivers
    switch(device.vendor_id)
    {
        case 0x1022: 
        {
            switch(device.device_id)
            {
                // am79c973 - network chip (default for vm, change in network settings => advanced => adaptor type)
                case 0x2000: 
                    // driver = (amd_am79c973 *)MemoryManager::active_memory_manager->malloc(sizeof(amd_am79c973));
                    // if(driver!=0)
                    //     new (driver)amd_am79c973(...);
                    print("am79c973; ");
                    break;

                // etc...
            }
        }
        break;

        case 0x8086: // Intel devices
            print("Intel; ");
            break;
        
        // etc...
    }

    switch(device.class_id)
    {
        //graphics
        case 0x03: 
        {
            switch(device.subclass_id)
            {
                //VGA
                case 0x00:
                    print("VGA; ");
                    break;
            }
        }
        break;
    }

    return driver;
}

BaseAddressRegister PCIController:: get_base_address_register(u16_t bus, u16_t device, u16_t function, u16_t bar)
{
    BaseAddressRegister res;
    u32_t header_type = read(bus, device, function, 0x0E) & 0x7F;
    u32_t maxBARS = 6 - (4*header_type);
    if(bar >= maxBARS)
        return res;

    u32_t bar_value = read(bus, device, function, 0x10 + 4*bar);
    res.type = (bar_value & 1) ? io : memory_mapping;
    
    u32_t temp;
    if(res.type == memory_mapping)
    {
        // right shift ignores the last type bit and &3 checks if 2nd and 3rd bit values are 00 == 0, 01 == 1 or 02 == 2
        switch((bar_value >> 1) & 0x3)
        {
            // we are not bothering about memory mapped in the code!
            case 0: // 00 - 32 bit mode
            case 1: // 01 - 20 bit mode
            case 2: // 10 - 64 bit mode
            break;
        }
        // check the 4th bit
        res.prefetchable = (((bar_value >> 0x3) & 0x1) == 0x1);

    }
    else // io
    {
        // remove the last 2 bits - type and reserved
        // bar_value is the port number we use for communication
        res.address = (u8_t *)(bar_value & ~0x3);
        res.prefetchable = 0;
    }
    return res;
}

