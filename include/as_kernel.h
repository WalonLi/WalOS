


#ifndef __AS_KERNEL_H__
#define __AS_KERNEL_H__


// HW interrupt vector table
#define HW_INT_VECTOR_DIVISION_BY_ERROR         0x0
#define HW_INT_VECTOR_DEBUGGER                  0x1
#define HW_INT_VECTOR_NMI                       0x2
#define HW_INT_VECTOR_BREAKPOINT                0x3
#define HW_INT_VECTOR_OVERFLOW                  0x4
#define HW_INT_VECTOR_BOUNDS                    0x5
#define HW_INT_VECTOR_INVALILD_OPCODE           0x6
#define HW_INT_VECTOR_COPR_NOT_AVAIL            0x7
#define HW_INT_VECTOR_DOUBLE_FAULT              0x8
#define HW_INT_VECTOR_COPR_SEG_OVERRUN          0x9
#define HW_INT_VECTOR_INVALID_TSS               0xA
#define HW_INT_VECTOR_SEG_NOT_PRESENT           0xB
#define HW_INT_VECTOR_STACK_FAULT               0xC
#define HW_INT_VECTOR_GERNERAL_PROTECT_FAULT    0xD
#define HW_INT_VECTOR_PAGE_FAULT                0xE
#define HW_INT_VECTOR_RESERVED                  0xF
#define HW_INT_VECTOR_MATH_FAULT                0x10
#define HW_INT_VECTOR_ALIGN_CHECK               0x11
#define HW_INT_VECTOR_MACHINE_CHECK             0x12
#define HW_INT_VECTOR_SIMD_FLOAT_POINT_EXECPT   0x13

void     _division_by_error();
void     _debugger();
void     _nmi();
void     _breakpoint();
void     _overflow();
void     _bounds();
void     _invalid_opcode();
void     _coprocessor_not_available();
void     _double_foult();
void     _coprocessor_segment_overrun();
void     _invalid_task_state_segment();
void     _segment_not_present();
void     _stack_fault();
void     _general_protected_fault();
void     _page_fault();
void     _reserved();
void     _math_fault();
void     _alignment_check();
void     _machine_check();
void     _simd_floating_point_exception();

// master 8259 controller
void    _hw_int00() ; // clock
void    _hw_int01() ; // keyboard
void    _hw_int02() ; // cascade to slaver
void    _hw_int03() ; // second serial
void    _hw_int04() ; // first serial
void    _hw_int05() ; // XT winchester
void    _hw_int06() ; // floppy
void    _hw_int07() ; // printer

// slaver 8259 controller
void    _hw_int08() ; // realtime clock
void    _hw_int09() ; // connect to master
void    _hw_int10() ;
void    _hw_int11() ;
void    _hw_int12() ;
void    _hw_int13() ; // FPU
void    _hw_int14() ; // AT winchester
void    _hw_int15() ;


void    _restart_process() ;


#endif
