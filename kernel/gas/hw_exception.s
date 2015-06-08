#
#   Provide by Walon Li
#   2015/6/01
#

.extern     hw_exception_handler

.code32

.text

# hardware execption
# wiki : http://en.wikipedia.org/wiki/Interrupt_descriptor_table
.global     division_by_error_exception
.global     debugger_exception
.global     nmi_exception
.global     breakpoint_exception
.global     overflow_exception
.global     bounds_exception
.global     invalid_opcode_exception
.global     coprocessor_not_available_exception
.global     double_foult_exception
.global     coprocessor_segment_overrun_exception
.global     invalid_task_state_segment_exception
.global     segment_not_present_exception
.global     stack_fault_exception
.global     general_protected_fault_exception
.global     page_fault_exception
.global     reserved
.global     math_fault_exception
.global     alignment_check_exception
.global     machine_check_exception
.global     simd_floating_point_exception

# hardware exception
# if push 0xffffffff at first, means no error code.
division_by_error_exception:
    push    $0xffffffff
    push    $0
    jmp     _exception
debugger_exception:
    push    $0xffffffff
    push    $1
    jmp     _exception
nmi_exception:
    push    $0xffffffff
    push    $2
    jmp     _exception
breakpoint_exception:
    push    $0xffffffff
    push    $3
    jmp     _exception
overflow_exception:
    push    $0xffffffff
    push    $4
    jmp     _exception
bounds_exception:
    push    $0xffffffff
    push    $5
    jmp     _exception
invalid_opcode_exception:
    push    $0xffffffff
    push    $6
    jmp     _exception
coprocessor_not_available_exception:
    push    $0xffffffff
    push    $7
    jmp     _exception
double_foult_exception:
    push    $8
    jmp     _exception
coprocessor_segment_overrun_exception:
    push    $0xffffffff
    push    $9
    jmp     _exception 
invalid_task_state_segment_exception:
    push    $10
    jmp     _exception
segment_not_present_exception:
    push    $11
    jmp     _exception
stack_fault_exception:
    push    $12
    jmp     _exception
general_protected_fault_exception:
    push    $13
    jmp     _exception    
page_fault_exception:
    push    $14
    jmp     _exception    
reserved:
    push    $0xffffffff
    push    $15
    jmp     _exception    
math_fault_exception:
    push    $0xffffffff
    push    $16
    jmp     _exception    
alignment_check_exception:
    push    $0xffffffff
    push    $17
    jmp     _exception    
machine_check_exception:
    push    $0xffffffff
    push    $18
    jmp     _exception    
simd_floating_point_exception:
    push    $0xffffffff
    push    $19
    jmp     _exception  

_exception:
    call    hw_exception_handler
    add     $8,     %esp
    hlt
    