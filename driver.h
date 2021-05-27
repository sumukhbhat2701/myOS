#ifndef DRIVER_H
#define DRIVER_H
#include "types.h"
class Driver
{
    public:
        Driver();
        ~Driver();

        virtual void activate();
        virtual int reset();
        virtual void deactivate();
};

class DriverManager
{
    private:
        Driver* drivers[255];
        u8_t num_of_drivers;
    public:
        DriverManager();
        ~DriverManager();
        void add_driver(Driver *);
        void activate_all();
};
#endif