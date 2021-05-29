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
    u32_t id = (0x1 << 31) | ((bus_id & 0xFF) << 16) | ((device_id & 0x1F) << 11) | ((function_id << 0x07) << 8) | ((register_offset) & 0xFC);
    command_port.write(id);
    u32_t result = data_port.read();
    // as u_32(int) is 4 bytes, 8 bits each but a device is represented by any bit, we must %4 and *8 to the register offset and get the corresponding bit in the result
    return result >> (8*(register_offset%4));
    

}

void PCIController::write(u16_t bus_id,u16_t device_id,u16_t function_id,u32_t register_offset, u32_t value)
{
    u32_t id = (0x1 << 31) | ((bus_id & 0xFF) << 16) | ((device_id & 0x1F) << 11) | ((function_id << 0x07) << 8) | ((register_offset) & 0xFC);
    command_port.write(id);
    data_port.write(value);
}

u8_t PCIController::device_has_functions(u16_t bus_id,u16_t device_id)
{
    return read(bus_id, device_id, 0, (0x0E) & (1 << 7));
}

void PCIController::select_drivers(DriverManager* driverManager)
{
    // list all the devices on the PCI which is managed by the driverManager
    // equivalent to "lspci" and "lsusb" command on the linux/unix shells.
    for(u8_t bus = 0; bus < 8; bus++)
    {
        for(u8_t device = 0; device < 32; device++)
        {
            u8_t num_of_functions = PCIController::device_has_functions(bus, device) ? 8 : 1;
            for(u8_t function = 0; function < num_of_functions; function++)
            {
                PCIDeviceDescriptor device_ = get_device_descriptor(bus, device, function);

                if(device_.vendor_id == 0 || device_.vendor_id == 0xFFFF)
                {
                    break;
                }

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
                print("\n");
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
