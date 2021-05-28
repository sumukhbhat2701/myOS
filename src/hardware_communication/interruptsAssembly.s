.set IRQ_BASE, 0x20

.section .text

# compiler given name to InterruptManager::interrupt_handler() function in the object file. Find it using "nm interrupts.o" (or same function name can be used if we make it extern "C")
# Reason: we need to invoke the function from assemly code, rather than from cpp because the compiler might bring change to the original code that was executing before the interrupt.  

# Syntax for naming _ZN<# of characters in namespace names><namespace names><# of characters in class names><class names> <function_name>E<some flag(s)>
.extern _ZN4myOS22hardware_communication16InterruptManager17interrupt_handlerEhj
.global _ZN4myOS22hardware_communication16InterruptManager24ignore_interrupt_requestEv

// ... function defn for InterruptManager::handle_exception_0x00() and InterruptManager::handle_exception_0x01()
.macro handle_exception num
.global _ZN4myOS22hardware_communication16InterruptManager21handle_exception_\num\()Ev
_ZN4myOS22hardware_communication16InterruptManager21handle_exception_\num\()Ev:
    movb $\num, (interrupt_id)
    jmp int_bottom
.endm

// ... function defn for InterruptManager::handle_interrupt_request_0x00() and for InterruptManager::handle_interrupt_request_0x01()
.macro handle_interrupt_request num
.global _ZN4myOS22hardware_communication16InterruptManager29handle_interrupt_request_\num\()Ev
_ZN4myOS22hardware_communication16InterruptManager29handle_interrupt_request_\num\()Ev:
    movb $\num + IRQ_BASE, (interrupt_id)
    jmp int_bottom
.endm

// call different interrupts' routine with thier id(we define it), 0x00+IRQ_BASE=0x20 - timer, 0x01+IRQ_BASE=0x21 - keyboard, 0x0C+IRQ_BASE=0x2C - mouse (IRQ_BASE = 0x20)
handle_interrupt_request 0x00
handle_interrupt_request 0x01
handle_interrupt_request 0x0C



int_bottom:
    # Of the process running before interrupted, keep a copy of all registers(a), data segment(ds), extra segment(es - point to the destination for a copy like while movs), fs and gs - commonly used by OS kernels to access thread-specific memory
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    # service the interrupt
    # call InterruptManager::interrupt_handler() with stack pointer(), interrupt_id as paramter 
    pushl %esp
    push (interrupt_id)
    call _ZN4myOS22hardware_communication16InterruptManager17interrupt_handlerEhj
    # update the stack pointer value to return value of InterruptManager::interrupt_handler()
    movl %eax, %esp

    # pop the content of registers as it was before interrupt in the reverse order of push
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN4myOS22hardware_communication16InterruptManager24ignore_interrupt_requestEv:
    # iret - interrupt return - specifies to the processor that interrupt is serviced and it can continue what it was doing before interrupted
    iret
    

.data
    interrupt_id: .byte 0