#ifndef DRIVER_H
#define DRIVER_H
#include <drivers/driver.h>
#include <common/types.h>

namespace myOS
{
    namespace drivers
    {
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
                myOS::common::u8_t num_of_drivers;
            public:
                DriverManager();
                ~DriverManager();
                void add_driver(Driver *);
                void activate_all();
        };
    }
}
#endif