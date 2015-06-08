/**
    Provide by Walon Li
    2015/6/8
**/

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "kernel/core.h"
#include "kernel/attribute.h"

// HW interrupt vector table
#define HW_EXCEPTION_DIVISION_BY_ERROR         0x0
#define HW_EXCEPTION_DEBUGGER                  0x1
#define HW_EXCEPTION_NMI                       0x2
#define HW_EXCEPTION_BREAKPOINT                0x3
#define HW_EXCEPTION_OVERFLOW                  0x4
#define HW_EXCEPTION_BOUNDS                    0x5
#define HW_EXCEPTION_INVALILD_OPCODE           0x6
#define HW_EXCEPTION_COPR_NOT_AVAIL            0x7
#define HW_EXCEPTION_DOUBLE_FAULT              0x8
#define HW_EXCEPTION_COPR_SEG_OVERRUN          0x9
#define HW_EXCEPTION_INVALID_TSS               0xA
#define HW_EXCEPTION_SEG_NOT_PRESENT           0xB
#define HW_EXCEPTION_STACK_FAULT               0xC
#define HW_EXCEPTION_GERNERAL_PROTECT_FAULT    0xD
#define HW_EXCEPTION_PAGE_FAULT                0xE
#define HW_EXCEPTION_RESERVED                  0xF
#define HW_EXCEPTION_MATH_FAULT                0x10
#define HW_EXCEPTION_ALIGN_CHECK               0x11
#define HW_EXCEPTION_MACHINE_CHECK             0x12
#define HW_EXCEPTION_SIMD_FLOAT_POINT_EXECPT   0x13

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

#endif
