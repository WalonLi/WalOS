#
#   Provide by Walon Li
#   2015/5/16
#


.code32

# extern function
.extern     cstart
.extern     hw_exception_handler
.extern     hw_irq_handler

# extern parameter
.extern     gdt_ptr
.extern     idt_ptr


.bss
#.lcomm    StackSpace,     1024*2
StackSpace:     .space      1024*2,        0
StackTop:


.text

.global     _start

# hardware execption
# wiki : http://en.wikipedia.org/wiki/Interrupt_descriptor_table
.global     _division_by_error
.global     _debugger
.global     _nmi
.global     _breakpoint
.global     _overflow
.global     _bounds
.global     _invalid_opcode
.global     _coprocessor_not_available
.global     _double_foult
.global     _coprocessor_segment_overrun
.global     _invalid_task_state_segment
.global     _segment_not_present
.global     _stack_fault
.global     _general_protected_fault
.global     _page_fault
.global     _reserved
.global     _math_fault
.global     _alignment_check
.global     _machine_check
.global     _simd_floating_point_exception

# 2 * i8259A interrupt controller
.global     _hw_int00
.global     _hw_int01
.global     _hw_int02
.global     _hw_int03
.global     _hw_int04
.global     _hw_int05
.global     _hw_int06
.global     _hw_int07
.global     _hw_int08
.global     _hw_int09
.global     _hw_int10
.global     _hw_int11
.global     _hw_int12
.global     _hw_int13
.global     _hw_int14
.global     _hw_int15

_start:
    mov     $StackTop,   %esp

    sgdtw   gdt_ptr
    call    cstart
    lgdtw   gdt_ptr

    lidtw   idt_ptr
    
    # jump to csinit to check GDT is work fine.
    # 0x8 is SELECTOR_KERNEL_CS
    ljmp    $0x8, $csinit
csinit:
    sti

    # for test hardware excpetion
    #ljmp    $0x40, $0
    #ud2 
    hlt
     


# hardware exception
# if push 0xffffffff at first, means no error code.
_division_by_error:
    push    $0xffffffff
    push    $0
    jmp     _exception
_debugger:
    push    $0xffffffff
    push    $1
    jmp     _exception
_nmi:
    push    $0xffffffff
    push    $2
    jmp     _exception
_breakpoint:
    push    $0xffffffff
    push    $3
    jmp     _exception
_overflow:
    push    $0xffffffff
    push    $4
    jmp     _exception
_bounds:
    push    $0xffffffff
    push    $5
    jmp     _exception
_invalid_opcode:
    push    $0xffffffff
    push    $6
    jmp     _exception
_coprocessor_not_available:
    push    $0xffffffff
    push    $7
    jmp     _exception
_double_foult:
    push    $8
    jmp     _exception
_coprocessor_segment_overrun:
    push    $0xffffffff
    push    $9
    jmp     _exception 
_invalid_task_state_segment:
    push    $10
    jmp     _exception
_segment_not_present:
    push    $11
    jmp     _exception
_stack_fault:
    push    $12
    jmp     _exception
_general_protected_fault:
    push    $13
    jmp     _exception    
_page_fault:
    push    $14
    jmp     _exception    
_reserved:
    push    $0xffffffff
    push    $15
    jmp     _exception    
_math_fault:
    push    $0xffffffff
    push    $16
    jmp     _exception    
_alignment_check:
    push    $0xffffffff
    push    $17
    jmp     _exception    
_machine_check:
    push    $0xffffffff
    push    $18
    jmp     _exception    
_simd_floating_point_exception:
    push    $0xffffffff
    push    $19
    jmp     _exception  

_exception:
    call    hw_exception_handler
    add     $8,     %esp
    hlt
    
    
# hardware interrupt
.macro master_8259 num
    pushl   $(\num)
    call    hw_irq_handler
    add     $4,     %esp
    hlt
.endm

.macro slaver_8259 num
    pushl   $(\num)
    call    hw_irq_handler
    add     $4,     %esp
    hlt
.endm

.align 16
_hw_int00:
    master_8259     0

_hw_int01:
    master_8259     1

_hw_int02:
    master_8259     2

_hw_int03:
    master_8259     3

_hw_int04:
    master_8259     4

_hw_int05:
    master_8259     5

_hw_int06:
    master_8259     6

_hw_int07:
    master_8259     7

_hw_int08:
    slaver_8259     8

_hw_int09:
    slaver_8259     9

_hw_int10:
    slaver_8259     10

_hw_int11:
    slaver_8259     11

_hw_int12:
    slaver_8259     12

_hw_int13:
    slaver_8259     13

_hw_int14:
    slaver_8259     14

_hw_int15:
    slaver_8259     15   
    
    
