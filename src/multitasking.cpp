#include<multitasking.h>

using namespace myOS;
using namespace myOS::common;


Task::Task(GlobalDescriptorTable* gdt, void entry_point())
{
    cpu_state = (CPUState *)(stack + 4096 - sizeof(CPUState));

    cpu_state->eax = 0;
    cpu_state->ebx = 0;
    cpu_state->ecx = 0;
    cpu_state->edx = 0;

    cpu_state->esi = 0;
    cpu_state->edi = 0;
    cpu_state->ebp = 0;

    // cpu_state->gs = 0; 
    // cpu_state->fs = 0; 
    // cpu_state->es = 0; 
    // cpu_state->ds = 0; 

    // cpu_state->error = 0;

    cpu_state->eip = (u32_t)entry_point;
    cpu_state->cs = gdt->compute_offset_codeSegmentSelector(); 
    cpu_state->eflags = 0x202;
    // cpu_state->esp = ;
    // cpu_state->ss; = 
}

Task::~Task()
{

}

TaskManager::TaskManager()
{
    num_of_tasks = 0;
    current_task = -1;
}

TaskManager::~TaskManager()
{
}

u8_t TaskManager::add_task(Task* task)
{
    if(num_of_tasks >= 256)
        return 0;
    tasks[num_of_tasks++] = task;
    return 1;
}

CPUState* TaskManager::schedule(CPUState* cpu_state)
{
    if(num_of_tasks <= 0)
        return cpu_state;

    if(current_task >= 0)
        tasks[current_task]->cpu_state = cpu_state;

    if(++current_task >= num_of_tasks)
        current_task %= num_of_tasks;
    
    return tasks[current_task]->cpu_state;
}