#ifndef DESKTOP_H
#define DESKTOP_H

#include<gui/widget.h>
#include<drivers/mouse.h>

namespace myOS
{
    namespace gui
    {
        // responsible for drawing of mouse - overwrites functions of mouse.h's mouseEventHandler
        class Desktop : public myOS::gui::CompositeWidget, public myOS::drivers::MouseEventHandler
        {
            protected:
                myOS::common::u32_t mouseX;
                myOS::common::u32_t mouseY;

            public:
                Desktop(myOS::common::s32_t w, myOS::common::s32_t h, myOS::common::u32_t r, myOS::common::u32_t g, myOS::common::u32_t b);
                ~Desktop();

                void draw(myOS::common::GraphicsContext* gc);
                void on_mouse_up(myOS::common::u8_t);
                void on_mouse_down(myOS::common::u8_t);
                void on_mouse_move(myOS::common::s32_t, myOS::common::s32_t);


        };
    }
}

#endif