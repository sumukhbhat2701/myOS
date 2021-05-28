#include <drivers/driver.h>
#include <common/types.h>

using namespace myOS::common;
using namespace myOS::drivers;

Driver::Driver()
{
    

}

Driver::~Driver() {}

void Driver::activate()
{

}

int Driver::reset()
{
    return 0;
}

void Driver::deactivate()
{

}

DriverManager::DriverManager()
{
    num_of_drivers = 0;
}

DriverManager::~DriverManager() {}

void DriverManager::add_driver(Driver* driver)
{
    drivers[num_of_drivers] = driver;
    num_of_drivers++;
}

void DriverManager::activate_all()
{
    for(u8_t i=0;i<num_of_drivers;i++)
    {
        drivers[i]->activate();
    }
}

