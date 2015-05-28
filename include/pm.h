/**
    Provide by Walon Li
    2015/5/22
**/

#ifndef WALOS_PM_H
#define WALOS_PM_H

#include "type.h"

#define GDT_SIZE    128
#define IDT_SIZE    256


typedef void (*interrupt_handler)() ;

typedef struct _DESCRIPTOR
{
    uint16_t  limit_low ;
    uint16_t  base_low ;
    uint8_t   base_mid ;
    uint8_t   attr1 ; //P:1 DPL:2 DT:1 TYPE:4
    uint8_t   limit_hight_attr2 ; // G:1 D:1 L:1 AVL:1 SegLimitHight:4
    uint8_t   base_hight ;
} DESCRIPTOR ;

typedef struct _GATE
{
    uint16_t  offset_low ;
    uint16_t  selector ;
    uint8_t   d_count ;
    uint8_t   attr ; //P:1 DPL:2 DT:1 TYPE:4
    uint16_t  offset_hight ;
} GATE ;


// Privilege
#define PRI_KRNL    0
#define PRI_TASK    1
#define PRI_USER    3


// GDT
// Descriptor index(define in loader)
#define INDEX_DUMMY     0
#define INDEX_FLAT_C    1
#define INDEX_FLAT_RW   2
#define INDEX_VIDEO     3

// Selector offset
#define SELECTOR_DUMMY      0
#define SELECTOR_FLAT_C     0x08
#define SELECTOR_FLAT_RW    0x10
#define SELECTOR_VIDEO      (0x18+3) // +RPL3

#define SELECTOR_KERNEL_CS  SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS  SELECTOR_FLAT_RW


// 32 bit
#define    DA_32      0x4000
#define    DA_LIMIT_4K 0x8000

// DPL(Discriptor Privilege Level)
#define    DA_DPL0    0x0
#define    DA_DPL1    0x20
#define    DA_DPL2    0x40
#define    DA_DPL3    0x60

// Segment type
#define    DA_DR      0x90
#define    DA_DRW     0x92
#define    DA_DRWA    0x93
#define    DA_C       0x98
#define    DA_CR      0x9a
#define    DA_CCO     0x9c
#define    DA_CCOR    0x9e

#define    DA_LDT     0x82
#define    DA_TaskGate 0x85
#define    DA_386TSS  0x89
#define    DA_386CGate 0x8c
#define    DA_386IGate 0x8e
#define    DA_386TGate 0x8f


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

void init_8259A() ;
void hw_exception_handler(int vector, int err_code, int eip, int cs, int eflags) ;
void init_idt_descs() ;
void init_idt_desc(unsigned char vector, uint8_t type, interrupt_handler handler, unsigned char privilege) ;


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


#endif
