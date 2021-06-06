#ifndef WIDGET_H
#define WIDGET_H
#include<common/types.h>
#include<common/graphicsContext.h>
#include<drivers/keyboard.h>

namespace myOS
{
    namespace gui
    {
        class Widget : public myOS::drivers::KeyboardEventHandler
        {
            protected:
                myOS::gui::Widget* parent;
                myOS::common::s32_t x, y, w, h;
                myOS::common::u8_t r, g, b;
                myOS::common::u8_t focussable;
            public:
                Widget(myOS::gui::Widget* parent, myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::s32_t w, myOS::common::s32_t h, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b);
                ~Widget();
                virtual myOS::common::u8_t contains_coordinate(myOS::common::s32_t x, myOS::common::s32_t y);
                virtual void get_focus(myOS::gui::Widget* widget);
                // &x and &y because it is pass by reference. Hence to find relative value of x and y and change it in function itself and retain its value once returns to the called function
                virtual void model_to_screen(myOS::common::s32_t &x, myOS::common::s32_t &y);
                virtual void draw(myOS::common::GraphicsContext* gc);
                virtual void on_mouse_down(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button);
                virtual void on_mouse_up(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button);
                virtual void on_mouse_move(myOS::common::s32_t oldX, myOS::common::s32_t oldY, myOS::common::s32_t newX, myOS::common::s32_t newY);

        };

        // children of objects of Widget class
        class CompositeWidget : public Widget
        {
            private:
                myOS::gui::Widget* children[100];
                myOS::common::u32_t num_of_children;
                myOS::gui::Widget* focussed_child;
            public:
                CompositeWidget(myOS::gui::Widget* parent, myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::s32_t w, myOS::common::s32_t h, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b);
                ~CompositeWidget();
                virtual myOS::common::u8_t add_child(myOS::gui::Widget* child);
                // these functions of parent class(Widget class) needs to be overwritten(overriden) - except model_to_screen()
                virtual void get_focus(myOS::gui::Widget* widget);
                virtual void draw(myOS::common::GraphicsContext* gc);
                virtual void on_mouse_down(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button);
                virtual void on_mouse_up(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button);
                virtual void on_mouse_move(myOS::common::s32_t oldX, myOS::common::s32_t oldY, myOS::common::s32_t newX, myOS::common::s32_t newY);

                virtual void on_key_down(char);
                virtual void on_key_up(char);
        };
    }
}

#endif