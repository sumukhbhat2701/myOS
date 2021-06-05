#include<drivers/vga.h>

using namespace myOS::drivers;
using namespace myOS::common;

void VGA::write_registers(myOS::common::u8_t* registers)
{
    // mischellaneous
    misc_port.write(*(registers++));

    // sequencer
    for(u8_t i = 0; i < 5; i++)
    {
        sequencer_index_port.write(i);
        sequencer_data_port.write(*(registers++));
    }

    // cathode ray tube controller - Need to follow mutual exclusion

    // you should unlock the CRTC and disable output before loading these registers, and afterwards restoring these to be nice for old monitors that are around.
    // set the 1st bit of the old value to 1(MSB of 8 bit(as it is 8 bit port) string) at 3rd index
    crtc_index_port.write(0x03);
    crtc_data_port.write(crtc_data_port.read() | 0x80);
    // set the 1st bit of the old value to 0(MSB of 8 bit(as it is 8 bit port) string) at 17th(0x11) index
    crtc_index_port.write(0x11);
    crtc_data_port.write(crtc_data_port.read() & ~0x80);

    // also set the corresponding bits of new values as above so that it is not overwritten when updated by new value 
    registers[0x03] |= 0x80;
    registers[0x11] &= ~0x80;

    for(u8_t i = 0; i < 25; i++)
    {
        crtc_index_port.write(i);
        crtc_data_port.write(*(registers++));
    }

    // graphics controller
    for(u8_t i = 0; i < 9; i++)
    {
        graphics_controller_index_port.write(i);
        graphics_controller_data_port.write(*(registers++));
    }

    // attribute controller
    for(u8_t i = 0; i < 21; i++)
    {
        attribute_controller_reset_port.read();
        attribute_controller_index_port.write(i);
        attribute_controller_write_port.write(*(registers++));
    }

    attribute_controller_reset_port.read();
    attribute_controller_index_port.write(0x20);

}

myOS::common::u8_t* VGA::get_frame_buffer_segment()
{
    graphics_controller_index_port.write(0x06);
    u8_t segment_number = ((graphics_controller_data_port.read() >> 2) & 0x03); 
    switch(segment_number)
    {
        case 0: return (u8_t *)0x00000;
        case 1: return (u8_t *)0xA0000;
        case 2: return (u8_t *)0xB0000;
        case 3: return (u8_t *)0xB8000;
        default: break;
    }
}

void VGA::put_pixel(myOS::common::u32_t x, myOS::common::u32_t y, myOS::common::u8_t color_index)
{
    u8_t* pixel_address = get_frame_buffer_segment() + 320*y + x; 
    *pixel_address = color_index;
}
myOS::common::u8_t VGA::get_color_index(myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b)
{
    // (0,0,0xA8) is blue
    if(r == 0 && g == 0 && b == 0xA8)
    {
        return 0x01;
    }

}


VGA::VGA():
 misc_port(0x3c2),
 crtc_index_port(0x3d4),
 crtc_data_port(0x3d5),
 sequencer_index_port(0x3c4),
 sequencer_data_port(0x3c5),
 graphics_controller_index_port(0x3ce),
 graphics_controller_data_port(0x3cf),
 attribute_controller_index_port(0x3c0),
 attribute_controller_read_port(0x3c1),
 attribute_controller_write_port(0x3c0),
 attribute_controller_reset_port(0x3da)
{

}

VGA::~VGA()
{

}


myOS::common::u8_t VGA::supports_mode(myOS::common::u32_t width, myOS::common::u32_t height, myOS::common::u32_t color_depth)
{
    // support only 320 x 200 x 8 dimensions
    return width == 320 && height == 200 && color_depth == 8;
}

myOS::common::u8_t VGA::set_mode(myOS::common::u32_t width, myOS::common::u32_t height, myOS::common::u32_t color_depth)
{
    // return 0 if dimensions not supported
    if(!supports_mode(width, height, color_depth))
        return 0;

    // From https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
    // 320 x 200 x 8 mode

    u8_t g_320x200x256[] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00,	0x00
    };

    write_registers(g_320x200x256);
    return 1;

}

void VGA::put_pixel(myOS::common::u32_t x, myOS::common::u32_t y, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b)
{
    put_pixel(x, y, get_color_index(r, g, b));
}
