#ifndef WINDOW_H
#define WINDOW_H

#include<gui/widget.h>
#include<drivers/mouse.h>

namespace myOS
{
    namespace gui
    {
        // draggable window(click+drag)
        class Window: public CompositeWidget
        {
            protected:
                myOS::common::u8_t dragging;
        
            public:
                Window(myOS::gui::Widget* parent, myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::s32_t w, myOS::common::s32_t h, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b);
                ~Window();
                void on_mouse_down(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button);
                void on_mouse_up(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button);
                void on_mouse_move(myOS::common::s32_t oldX, myOS::common::s32_t oldY, myOS::common::s32_t newX, myOS::common::s32_t newY);
        };
    }
}

#endif