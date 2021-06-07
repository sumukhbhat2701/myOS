#ifndef MULTITASKIG_H
#define MULTITASKIG_H

#include<gdt.h>

namespace myOS
{
    // current cpu register values
    struct CPUState
    {
        myOS::common::u32_t eax;
        myOS::common::u32_t ebx;
        myOS::common::u32_t ecx;
        myOS::common::u32_t edx;

        myOS::common::u32_t esi;
        myOS::common::u32_t edi;
        myOS::common::u32_t ebp;

        // myOS::common::u32_t gs;
        // myOS::common::u32_t fs;
        // myOS::common::u32_t es;
        // myOS::common::u32_t ds;

        myOS::common::u32_t error;

        myOS::common::u32_t eip;
        myOS::common::u32_t cs;
        myOS::common::u32_t eflags;
        myOS::common::u32_t esp;
        myOS::common::u32_t ss;
    }__attribute__((packed));
    
    class Task
    {
        friend class TaskManager;
        private:
            myOS::common::u8_t stack[4096]; // 4KB
            CPUState* cpu_state;

        public:
            Task(GlobalDescriptorTable* gdt, void entry_point());
            ~Task();

    };

    class TaskManager
    {
        private:
            Task* tasks[256];
            myOS::common::s32_t num_of_tasks;
            myOS::common::s32_t current_task;
        public:
            TaskManager();
            ~TaskManager();
            myOS::common::u8_t add_task(Task* task);
            CPUState* schedule(CPUState* cpu_state);
    };
}


#endif