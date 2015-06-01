#
#   Provide by Walon Li
#   2015/6/01
#

.extern     hw_exception_handler

.code32

.text

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
    