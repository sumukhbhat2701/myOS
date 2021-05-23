.set IRQ_BASE, 0x20

.section .text

# compiler given name to InterruptManager::interrupt_handler() function in the object file. Find it using "nm interrupts.o" 
# Reason: we need to invoke the function from assemly code, rather than from cpp because the compiler might bring change to the original code that was executing before the interrupt.  
.extern _ZN16InterruptManager17interrupt_handlerEhj
.global _ZN16InterruptManager24ignore_interrupt_requestEv

// ... for InterruptManager::handle_exception_0x00() and InterruptManager::handle_exception_0x01()
.macro handle_exception num
.global _ZN16InterruptManager21handle_exception_\num\()Ev
_ZN16InterruptManager21handle_exception_\num\()Ev:
    movb $\num, (interrupt_id)
    jmp int_bottom
.endm

// ... for InterruptManager::handle_interrupt_request_0x00() and for InterruptManager::handle_interrupt_request_0x01()
.macro handle_interrupt_request num
.global _ZN16InterruptManager29handle_interrupt_request_\num\()Ev
_ZN16InterruptManager29handle_interrupt_request_\num\()Ev:
    movb $\num + IRQ_BASE, (interrupt_id)
    jmp int_bottom
.endm

handle_interrupt_request 0x00
handle_interrupt_request 0x01



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
    call _ZN16InterruptManager17interrupt_handlerEhj
    # update the stack pointer value to return value of InterruptManager::interrupt_handler()
    movl %eax, %esp

    # pop the content of registers as it was before interrupt in the reverse order of push
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN16InterruptManager24ignore_interrupt_requestEv:
    # iret - interrupt return - specifies to the processor that interrupt is serviced and it can continue what it was doing before interrupted
    iret
    

.data
    interrupt_id: .byte 0