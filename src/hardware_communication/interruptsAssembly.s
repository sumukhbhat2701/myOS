.set IRQ_BASE, 0x20

.section .text

# compiler given name to InterruptManager::interrupt_handler() function in the object file. Find it using "nm interrupts.o" (or same function name can be used if we make it extern "C")
# Reason: we need to invoke the function from assemly code, rather than from cpp because the compiler might bring change to the original code that was executing before the interrupt.  

# Syntax for naming _ZN<# of characters in namespace names><namespace names><# of characters in class names><class names> <function_name>E<some flag(s)>
.extern _ZN4myOS22hardware_communication16InterruptManager17interrupt_handlerEhj
.global _ZN4myOS22hardware_communication16InterruptManager24ignore_interrupt_requestEv

# ... function defn for InterruptManager::handle_exception_0x00() and InterruptManager::handle_exception_0x01()
.macro handle_exception num
.global _ZN4myOS22hardware_communication16InterruptManager21handle_exception_\num\()Ev
_ZN4myOS22hardware_communication16InterruptManager21handle_exception_\num\()Ev:
    movb $\num, (interrupt_id)
    jmp int_bottom
.endm

# ... function defn for InterruptManager::handle_interrupt_request_0x00() and for InterruptManager::handle_interrupt_request_0x01()
.macro handle_interrupt_request num
.global _ZN4myOS22hardware_communication16InterruptManager29handle_interrupt_request_\num\()Ev
_ZN4myOS22hardware_communication16InterruptManager29handle_interrupt_request_\num\()Ev:
    movb $\num + IRQ_BASE, (interrupt_id)
    pushl $0   # for u32_t error; in multitasking.h
    jmp int_bottom
.endm

# call different interrupts' routine with thier id(we define it), 0x00+IRQ_BASE=0x20 - timer, 0x01+IRQ_BASE=0x21 - keyboard, 0x0C+IRQ_BASE=0x2C - mouse (IRQ_BASE = 0x20)
handle_interrupt_request 0x00
handle_interrupt_request 0x01
handle_interrupt_request 0x02
handle_interrupt_request 0x03
handle_interrupt_request 0x04
handle_interrupt_request 0x05
handle_interrupt_request 0x06
handle_interrupt_request 0x07
handle_interrupt_request 0x08
handle_interrupt_request 0x09
handle_interrupt_request 0x0A
handle_interrupt_request 0x0B
handle_interrupt_request 0x0C
handle_interrupt_request 0x0D
handle_interrupt_request 0x0E
handle_interrupt_request 0x0F
handle_interrupt_request 0x31
handle_interrupt_request 0x80

handle_exception 0x00
handle_exception 0x01
handle_exception 0x02
handle_exception 0x03
handle_exception 0x04
handle_exception 0x05
handle_exception 0x06
handle_exception 0x07
handle_exception 0x08
handle_exception 0x09
handle_exception 0x0A
handle_exception 0x0B
handle_exception 0x0C
handle_exception 0x0D
handle_exception 0x0E
handle_exception 0x0F
handle_exception 0x10
handle_exception 0x11
handle_exception 0x12
handle_exception 0x13



int_bottom:
    # Of the process running before interrupted, keep a copy of all registers(a), data segment(ds), extra segment(es - point to the destination for a copy like while movs), fs and gs - commonly used by OS kernels to access thread-specific memory
    # pusha
    # pushl %ds
    # pushl %es
    # pushl %fs
    # pushl %gs

    # For multitasking in multitasking.h(instead of above 5 lines):
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # service the interrupt
    # call InterruptManager::interrupt_handler() with stack pointer(), interrupt_id as paramter 
    pushl %esp
    push (interrupt_id)
    call _ZN4myOS22hardware_communication16InterruptManager17interrupt_handlerEhj
    # update the stack pointer value to return value of InterruptManager::interrupt_handler()
    movl %eax, %esp

    # pop the content of registers as it was before interrupt in the reverse order of push
    # popl %gs
    # popl %fs
    # popl %es
    # popl %ds
    # popa

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp

    add $4, %esp

.global _ZN4myOS22hardware_communication16InterruptManager24ignore_interrupt_requestEv
_ZN4myOS22hardware_communication16InterruptManager24ignore_interrupt_requestEv:
    # iret - interrupt return - specifies to the processor that interrupt is serviced and it can continue what it was doing before interrupted
    iret
    

.data
    interrupt_id: .byte 0