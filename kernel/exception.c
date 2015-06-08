/**
    Provide by Walon Li
    2015/6/8
**/

#include "kernel/global.h"
#include "kernel/exception.h"
#include "lib/common.h"

// hw exception and handler initial function
void hw_exception_handler(int vector, int err_code, int eip, int cs, int eflags)
{
    // uint16_t color = 0x74 ;
    char *err_msg[] = {"##_division_by_error",
                       "##_debugger",
                       "##_nmi",
                       "##_breakpoint",
                       "##_overflow",
                       "##_bounds",
                       "##_invalid_opcode",
                       "##_coprocessor_not_available",
                       "##_double_foult",
                       "##_coprocessor_segment_overrun",
                       "##_invalid_task_state_segment",
                       "##_segment_not_present",
                       "##_stack_fault",
                       "##_general_protected_fault",
                       "##_page_fault",
                       "##_reserved",
                       "##_math_fault",
                       "##_alignment_check",
                       "##_machine_check",
                       "##_simd_floating_point_exception"} ;
    position = 0 ;
    show_msg_ext("Exception : ", 0x74) ;
    show_msg_ext(err_msg[vector], 0x74) ;
    show_msg("\n") ;

    char buffer[80] = { 0 };

    show_msg_ext("eflags:", 0x74) ;
    show_msg_ext(itoa_base(eflags, buffer, 16), 0x74) ;

    show_msg_ext(" cs:", 0x74) ;
    show_msg_ext(itoa_base(cs, buffer, 16), 0x74) ;

    show_msg_ext(" eip:", 0x74) ;
    show_msg_ext(itoa_base(eip, buffer, 16), 0x74) ;

    show_msg_ext(" err_code:", 0x74) ;
    show_msg_ext(itoa_base(err_code, buffer, 16), 0x74) ;

}

void init_hw_exception_idt()
{
    // initial all interrupt gate
    init_idt_desc(HW_EXCEPTION_DIVISION_BY_ERROR, DA_386IGate, division_by_error_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_DEBUGGER, DA_386IGate, debugger_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_NMI, DA_386IGate, nmi_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_BREAKPOINT, DA_386IGate, breakpoint_exception, PRI_USER) ;
    init_idt_desc(HW_EXCEPTION_OVERFLOW, DA_386IGate, overflow_exception, PRI_USER) ;
    init_idt_desc(HW_EXCEPTION_BOUNDS, DA_386IGate, bounds_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_INVALILD_OPCODE, DA_386IGate, invalid_opcode_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_COPR_NOT_AVAIL, DA_386IGate, coprocessor_not_available_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_DOUBLE_FAULT, DA_386IGate, double_foult_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_COPR_SEG_OVERRUN, DA_386IGate, coprocessor_segment_overrun_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_INVALID_TSS, DA_386IGate, invalid_task_state_segment_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_SEG_NOT_PRESENT, DA_386IGate, segment_not_present_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_STACK_FAULT, DA_386IGate, stack_fault_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_GERNERAL_PROTECT_FAULT, DA_386IGate, general_protected_fault_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_PAGE_FAULT, DA_386IGate, page_fault_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_RESERVED, DA_386IGate, reserved, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_MATH_FAULT, DA_386IGate, math_fault_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_ALIGN_CHECK, DA_386IGate, alignment_check_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_MACHINE_CHECK, DA_386IGate, machine_check_exception, PRI_KRNL) ;
    init_idt_desc(HW_EXCEPTION_SIMD_FLOAT_POINT_EXECPT, DA_386IGate, simd_floating_point_exception, PRI_KRNL) ;
}

