#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include<common/types.h>

namespace myOS
{
    struct Bookkeeping
    {
        struct Bookkeeping *next;
        struct Bookkeeping *prev;
        myOS::common::u8_t allocated;
        myOS::common::size_t size;
    };

    class MemoryManager
    {
        protected:
            Bookkeeping* head;
        public:
            static MemoryManager* active_memory_manager;
            MemoryManager(myOS::common::size_t head, myOS::common::size_t size);
            ~MemoryManager();
            
            void* malloc(myOS::common::size_t size);
            void free(void* ptr);

    };
}

void* operator new(unsigned size);
void* operator new[](unsigned size);

void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif