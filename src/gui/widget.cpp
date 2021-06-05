#include<gui/widget.h>

using namespace myOS::common;
using namespace myOS::gui;

Widget::Widget(Widget* parent, s32_t x, s32_t y, s32_t z, u8_t r, u8_t g, u8_t b)
{
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->r = r;
    this->g = g;
    this->b = b;
    this->focussable = 1;
}

Widget::~Widget()
{

}

void Widget::get_focus(Widget* widget)
{
    if(parent!=0)
    {
        parent->get_focus(widget);
    }
}

// &x and &y because it is pass by reference. Hence to find relative value of x and y and change it in function itself and retain its value once returns to the called function
void Widget::model_to_screen(s32_t &x, s32_t &y)
{
    if(parent!=0)
    {
        parent->model_to_screen(x,y);
    }
    x += this->x;
    y += this->y;
}

void Widget::draw(GraphicsContext* gc)
{
    s32_t X = 0, Y = 0;
    model_to_screen(X, Y);
    gc->fill_rectangle(X, Y, w, h, r, g, b);
}

void Widget::on_mouse_down(s32_t x, s32_t y)
{
    if(focussable)
    {
        get_focus(this);
    }
}

void Widget::on_mouse_up(s32_t x, s32_t y)
{

}

void Widget::on_mouse_move(s32_t oldX, s32_t oldY, s32_t newX, s32_t newY)
{

}

void Widget::on_key_down(char* s)
{

}

void Widget::on_key_up(char* s)
{

}



CompositeWidget::CompositeWidget(Widget* parent, s32_t x, s32_t y, s32_t z, u8_t r, u8_t g, u8_t b)
{
    focussed_child = 0;
    num_of_children = 0;
}

CompositeWidget::~CompositeWidget()
{

}

void CompositeWidget::get_focus(Widget* widget)
{
    this->focussed_child = widget;
    if(parent!=0)
    {
        parent->get_focus(this);
    }
}

void CompositeWidget::draw(GraphicsContext* gc)
{
    // draw a background for itself
    Widget::draw(gc);
    // draw the children
    for(u32_t i = num_of_children-1; i>=0; i--)
    {
        children[i]->draw(gc);
    }
}

void CompositeWidget::on_mouse_down(s32_t x, s32_t y)
{
    for(u32_t i = 0; i<num_of_children; i--)
    {
        if(children[i]->contains_coordinate(x -this->x, y - this->y))
        {
            children[i]->on_mouse_down(x -this->x, y - this->y);
            break;
        }
    }
}

void CompositeWidget::on_mouse_up(s32_t x, s32_t y)
{
    for(u32_t i = 0; i<num_of_children; i--)
    {
        if(children[i]->contains_coordinate(x -this->x, y - this->y))
        {
            children[i]->on_mouse_up(x -this->x, y - this->y);
            break;
        }
    }
}

void CompositeWidget::on_mouse_move(s32_t oldX, s32_t oldY, s32_t newX, s32_t newY)
{
    u32_t first_child = -1;
    for(u32_t i = 0; i<num_of_children; i--)
    {
        if(children[i]->contains_coordinate(oldX -this->x, oldX - this->y))
        {
            children[i]->on_mouse_move(oldX -this->x, oldY - this->y, newX - this->x, newY - this->y);
            first_child = i;
            break;
        }
    }

    for(u32_t i = 0; i<num_of_children; i--)
    {
        if(children[i]->contains_coordinate(newX -this->x, newY - this->y))
        {
            if(first_child != i)
            {
                children[i]->on_mouse_move(oldX -this->x, oldY - this->y, newX - this->x, newY - this->y);
            }
            break;
        }
    }
}

void CompositeWidget::on_key_down(char* s)
{
    if(focussed_child!=0)
    {
        focussed_child->on_key_down(s);
    }
}

void CompositeWidget::on_key_up(char* s)
{
    if(focussed_child!=0)
    {
        focussed_child->on_key_up(s);
    }
}
