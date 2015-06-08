/**
    Provide by Walon Li
    2015/6/8
**/

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "kernel/core.h"
#include "kernel/attribute.h"

// HW interrupt vector table
#define INT_VECTOR_DIVISION_BY_ERROR         0x0
#define INT_VECTOR_DEBUGGER                  0x1
#define INT_VECTOR_NMI                       0x2
#define INT_VECTOR_BREAKPOINT                0x3
#define INT_VECTOR_OVERFLOW                  0x4
#define INT_VECTOR_BOUNDS                    0x5
#define INT_VECTOR_INVALILD_OPCODE           0x6
#define INT_VECTOR_COPR_NOT_AVAIL            0x7
#define INT_VECTOR_DOUBLE_FAULT              0x8
#define INT_VECTOR_COPR_SEG_OVERRUN          0x9
#define INT_VECTOR_INVALID_TSS               0xA
#define INT_VECTOR_SEG_NOT_PRESENT           0xB
#define INT_VECTOR_STACK_FAULT               0xC
#define INT_VECTOR_GERNERAL_PROTECT_FAULT    0xD
#define INT_VECTOR_PAGE_FAULT                0xE
#define INT_VECTOR_RESERVED                  0xF
#define INT_VECTOR_MATH_FAULT                0x10
#define INT_VECTOR_ALIGN_CHECK               0x11
#define INT_VECTOR_MACHINE_CHECK             0x12
#define INT_VECTOR_SIMD_FLOAT_POINT_EXECPT   0x13

void     division_by_error_exception();
void     debugger_exception();
void     nmi_exception();
void     breakpoint_exception();
void     overflow_exception();
void     bounds_exception();
void     invalid_opcode_exception();
void     coprocessor_not_available_exception();
void     double_foult_exception();
void     coprocessor_segment_overrun_exception();
void     invalid_task_state_segment_exception();
void     segment_not_present_exception();
void     stack_fault_exception();
void     general_protected_fault_exception();
void     page_fault_exception();
void     reserved();
void     math_fault_exception();
void     alignment_check_exception();
void     machine_check_exception();
void     simd_floating_point_exception();

void    init_hw_exception_idt() ;
#endif
