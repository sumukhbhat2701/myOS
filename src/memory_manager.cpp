#include<memory_manager.h>

using namespace myOS;
using namespace myOS::common;

// static objects need to be compulsorily be initialzed
MemoryManager* MemoryManager::active_memory_manager = 0;

MemoryManager::MemoryManager(size_t start, myOS::common::size_t size)
{
    active_memory_manager = this;

    if(size <= sizeof(struct Bookkeeping))
        head = 0;
    else
    {
        head = (struct Bookkeeping *)start;
        head->allocated = 0;
        head->prev = 0;
        head->next = 0;
        head->size = size - sizeof(struct Bookkeeping);
    }
    


}

MemoryManager::~MemoryManager()
{
    if(active_memory_manager == this)
        active_memory_manager = 0;
}


void* MemoryManager::malloc(myOS::common::size_t size)
{
    // first fit technique
    struct Bookkeeping* allocated = 0;
    for(struct Bookkeeping* temp = head; temp!=0; temp = temp->next)
        if(temp->size > size && !temp->allocated)
            allocated = temp;
    
    // failed allocation
    if(allocated == 0)
        return 0;
    
    if(allocated->size < size+sizeof(struct Bookkeeping) + 1)
        allocated->allocated = 1;
    else
    {
        struct Bookkeeping* temp = (struct Bookkeeping*)((size_t)allocated + sizeof(struct Bookkeeping) + size);
        temp->allocated = 0;
        temp->size = allocated->size - sizeof(struct Bookkeeping);
        temp->prev = allocated;
        temp->next = allocated->next;
        if(temp->next != 0)
            temp->next->prev = temp;

        allocated->size = size;
        allocated->next = temp;
    }

    allocated->allocated = 1;
    return (void *)(((size_t)allocated) + sizeof(struct Bookkeeping));

}

void MemoryManager::free(void* ptr)
{
    struct Bookkeeping* temp = (struct Bookkeeping *)((size_t)ptr - sizeof(struct Bookkeeping));
    temp->allocated = 0;

    if(temp->prev !=0 && !temp->prev->allocated)
    {
        temp->prev->next = temp->next;
        temp->prev->size += temp->size + sizeof(struct Bookkeeping);
        if(temp->next != 0)
            temp->next->prev = temp->prev;
        
        temp = temp->prev;
    }

    if(temp->next !=0 && !temp->next->allocated)
    {
        temp->size += temp->next->size + sizeof(struct Bookkeeping);
        temp->next = temp->next->next;
        if(temp->next != 0)
            temp->next->prev = temp;
    }
}

void* operator new(unsigned size)
{   
    if(myOS::MemoryManager::active_memory_manager == 0)
        return 0;
    return myOS::MemoryManager::active_memory_manager->malloc(size);
}

void* operator new[](unsigned size)
{
    if(myOS::MemoryManager::active_memory_manager == 0)
        return 0;
    return myOS::MemoryManager::active_memory_manager->malloc(size);
}

void* operator new(unsigned size, void* ptr)
{   
    return ptr;
}

void* operator new[](unsigned size, void* ptr)
{
    return ptr; 
}


void operator delete(void* ptr)
{
    if(myOS::MemoryManager::active_memory_manager != 0)
        myOS::MemoryManager::active_memory_manager->free(ptr);
}

void operator delete[](void* ptr)
{
    if(myOS::MemoryManager::active_memory_manager != 0)
        myOS::MemoryManager::active_memory_manager->free(ptr);
}
