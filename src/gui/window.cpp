#include<gui/window.h>

using namespace myOS;
using namespace myOS::common;
using namespace myOS::gui;

Window::Window(myOS::gui::Widget* parent, myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::s32_t w, myOS::common::s32_t h, myOS::common::u8_t r, myOS::common::u8_t g, myOS::common::u8_t b)
: CompositeWidget(parent, x, y, w, h, r, g, b)
{
    dragging = 0;
}

Window::~Window()
{

}

void Window::on_mouse_down(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button)
{
    dragging = (button == 1);
    CompositeWidget::on_mouse_down(x,y,button);
}

void Window::on_mouse_up(myOS::common::s32_t x, myOS::common::s32_t y, myOS::common::u8_t button)
{
    dragging = 0;
    CompositeWidget::on_mouse_up(x,y,button);
}

void Window::on_mouse_move(myOS::common::s32_t oldX, myOS::common::s32_t oldY, myOS::common::s32_t newX, myOS::common::s32_t newY)
{
    // if mouse is pressed(dragged = 1) and moved, move the window created as well to new position of mouse
    if(dragging)
    {
        this->x += (newX - oldX);
        this->y += (newY - oldY);
    }
    CompositeWidget::on_mouse_move(oldX, oldY,newX, newY);
}
