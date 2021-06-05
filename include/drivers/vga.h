#ifndef VGA_H
#define VGA_H

#include <common/types.h>
#include <hardware_communication/interrupts.h>
#include <hardware_communication/port.h>
#include <drivers/driver.h>

namespace myOS
{
    namespace drivers
    {
        // VGA - Video Graphics Array - standard connector used for computer video output
        class VGA
        {
            protected:
                // mischellaneous
                hardware_communication::Port_8bits misc_port;
                // cathode ray tube controller
                hardware_communication::Port_8bits crtc_index_port;
                hardware_communication::Port_8bits crtc_data_port;
                // sequencer
                hardware_communication::Port_8bits sequencer_index_port;
                hardware_communication::Port_8bits sequencer_data_port;
                // graphics controller
                hardware_communication::Port_8bits graphics_controller_index_port;
                hardware_communication::Port_8bits graphics_controller_data_port;
                // attribute controller
                hardware_communication::Port_8bits attribute_controller_index_port;
                hardware_communication::Port_8bits attribute_controller_read_port;
                hardware_communication::Port_8bits attribute_controller_write_port;
                hardware_communication::Port_8bits attribute_controller_reset_port;

                void write_registers(myOS::common::u8_t* registers);
                // pointer to proper place offset in the buffer
                myOS::common::u8_t* get_frame_buffer_segment();
                // get index of the corresponding (r, g, b) entries
                virtual myOS::common::u8_t get_color_index(myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b);


            public:
                VGA();
                ~VGA();

                virtual myOS::common::u8_t supports_mode(myOS::common::u32_t width, myOS::common::u32_t height, myOS::common::u32_t color_depth);
                virtual myOS::common::u8_t set_mode(myOS::common::u32_t width, myOS::common::u32_t height, myOS::common::u32_t color_depth);
                virtual void put_pixel(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b);
                // get index corresponding to (r, g, b) entries and use that instead of r, g, b values explicilty to put_pixels
                virtual void put_pixel(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t color_index);
                virtual void fill_rectangle(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::s32_t w, myOS::common::s32_t h, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b);

        };
    }
}

#endif