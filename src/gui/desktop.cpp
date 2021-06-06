#include<gui/desktop.h>
using namespace myOS::common;
using namespace myOS::gui;

Desktop::Desktop(s32_t w, s32_t h, u32_t r, u32_t g, u32_t b)
: CompositeWidget(0, 0, 0, w, h, r, g, b),
MouseEventHandler()
{
    // current position at the center of screen
    mouseX = w/2;
    mouseY = h/2;
}
Desktop::~Desktop()
{
    
}

void Desktop::draw(GraphicsContext* gc)
{
    CompositeWidget::draw(gc);

    // draw a white + sign on the screen representing the mouse cursor
    for(u8_t i = 0; i<4; i++)
    {
        gc->put_pixel(mouseX-i, mouseY, 0xFF, 0xFF, 0xFF);
        gc->put_pixel(mouseX+i, mouseY, 0xFF, 0xFF, 0xFF);
        gc->put_pixel(mouseX, mouseY-i, 0xFF, 0xFF, 0xFF);
        gc->put_pixel(mouseX, mouseY+i, 0xFF, 0xFF, 0xFF);
    }
}
void Desktop::on_mouse_up(u8_t button)
{
    CompositeWidget::on_mouse_up(mouseX, mouseY, button);
}
void Desktop::on_mouse_down(u8_t button)
{
    CompositeWidget::on_mouse_down(mouseX, mouseY, button);
}
void Desktop::on_mouse_move(s32_t x, s32_t y)
{
    // slower the mouse move speed by 4 times
    x/=4;
    y/=4;

    // check if the new position is within limits and set it
    s32_t newX = mouseX + x;
    if(newX < 0) newX = 0;
    if(newX >= w) newX = w-1;

    s32_t newY = mouseY + y;
    if(newY < 0) newY = 0;
    if(newY >= h) newY = h-1;

    CompositeWidget::on_mouse_move(mouseX, mouseY, newX, newY);

    mouseX = newX;
    mouseY = newY;
}