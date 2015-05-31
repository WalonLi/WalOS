/**
    Provide by Walon Li
    2015/5/22
**/

#ifndef WALOS_PM_H
#define WALOS_PM_H

#include "type.h"

#define GDT_SIZE    128
#define IDT_SIZE    256
#define LDT_SIZE    2


typedef void (*interrupt_handler)() ;

typedef struct _DESCRIPTOR
{
    uint16_t  limit_low ;
    uint16_t  base_low ;
    uint8_t   base_mid ;
    uint8_t   attr1 ; //P:1 DPL:2 DT:1 TYPE:4
    uint8_t   limit_high_attr2 ; // G:1 D:1 L:1 AVL:1 SegLimitHight:4
    uint8_t   base_high ;
} DESCRIPTOR ;

typedef struct _GATE
{
    uint16_t  offset_low ;
    uint16_t  selector ;
    uint8_t   d_count ;
    uint8_t   attr ; //P:1 DPL:2 DT:1 TYPE:4
    uint16_t  offset_high ;
} GATE ;

typedef struct _TSS
{
    uint32_t    back ;
    uint32_t    esp0 ;
    uint32_t    ss0 ;
    uint32_t    esp1 ;
    uint32_t    ss1 ;
    uint32_t    esp2 ;
    uint32_t    ss2 ;
    uint32_t    cr3 ;
    uint32_t    eip ;
    uint32_t    flags ;
    uint32_t    eax ;
    uint32_t    ecx ;
    uint32_t    edx ;
    uint32_t    ebx ;
    uint32_t    esp ;
    uint32_t    ebp ;
    uint32_t    esi ;
    uint32_t    edi ;
    uint32_t    es ;
    uint32_t    cs ;
    uint32_t    ss ;
    uint32_t    ds ;
    uint32_t    fs ;
    uint32_t    gs ;
    uint32_t    ldt ;
    uint32_t    trap ;
    uint32_t    iobase ;
} TSS ;



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
#define INDEX_TSS       4
#define INDEX_LDT_FIRST 5

// Selector offset
#define SELECTOR_DUMMY      0
#define SELECTOR_FLAT_C     0x08
#define SELECTOR_FLAT_RW    0x10
#define SELECTOR_VIDEO      (0x18+3) // +RPL3
#define SELECTOR_TSS        0x20
#define SELECTOR_LDT_FIRST  0x28

#define SELECTOR_KERNEL_CS  SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS  SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS  SELECTOR_VIDEO


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

#define     SA_RPL_MASK     0xfffc
#define     SA_RPL0         0
#define     SA_RPL1         1
#define     SA_RPL2         2
#define     SA_RPL3         3
#define     SA_TI_MASK      0xfffb
#define     SA_TIG          0
#define     SA_TIL          4

#define     RPL_KRNL    SA_RPL0
#define     RPL_TASK    SA_RPL1
#define     RPL_USER    SA_RPL3


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

#define vir2phys(seg_base, vir) (uint32_t)(((uint32_t)seg_base) + (uint32_t)(vir))

void init_8259A() ;
void hw_exception_handler(int vector, int err_code, int eip, int cs, int eflags) ;
void hw_irq_handler(int irq) ;
void init_idt_descs() ;
void init_8259_irq() ;
void init_tss() ;
void init_idt_desc(unsigned char vector, uint8_t type, interrupt_handler handler, unsigned char privilege) ;

void init_gdt_desc(DESCRIPTOR *desc, uint32_t base, uint32_t limit, uint16_t attr) ;
#define init_ldt_desc init_gdt_desc

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
