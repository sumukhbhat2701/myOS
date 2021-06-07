#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H
#include<drivers/vga.h>

namespace myOS
{
    namespace common
    {
        // use vga as base from graphics
        typedef myOS::drivers::VGA GraphicsContext;
    }
}

#endif