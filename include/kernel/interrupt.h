/**
    Provide by Walon Li
    2015/6/8
**/

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "kernel/core.h"
#include "kernel/attribute.h"


// HARDWARE

typedef void (*irq_handler)(int irq) ;
// master 8259a
#define INT_M_CTRL          0x20
#define INT_M_CTRL_MASK     0x21
// slaver 8259a
#define INT_S_CTRL          0xA0
#define INT_S_CTRL_MASK     0xA1

#define INT_VECTOR_IRQ0     0x20
#define INT_VECTOR_IRQ8     0x28

#define IRQ_CLOCK           0x0
#define IRQ_KEYBOARD        0x1
#define IRQ_M_TO_S          0x2 // Master to Slaver
#define IRQ_SECOND_SERIAL   0x3
#define IRQ_FIRST_SERAIL    0x4
#define IRQ_XT              0x5
#define IRQ_FLOPPY          0x6
#define IRQ_PRINTER         0x7
#define IRQ_RT_CLOCK        0x8
#define IRQ_S_TO_M          0x9
#define IRQ_FPU             0xd
#define IRQ_AT              0xe

#define IRQ_CNT             16

// master 8259 controller
void    hw_int00() ; // clock
void    hw_int01() ; // keyboard
void    hw_int02() ; // cascade to slaver
void    hw_int03() ; // second serial
void    hw_int04() ; // first serial
void    hw_int05() ; // XT winchester
void    hw_int06() ; // floppy
void    hw_int07() ; // printer

// slaver 8259 controller
void    hw_int08() ; // realtime clock
void    hw_int09() ; // connect to master
void    hw_int10() ;
void    hw_int11() ;
void    hw_int12() ;
void    hw_int13() ; // FPU
void    hw_int14() ; // AT winchester
void    hw_int15() ;

void    enable_irq(int irq) ;
void    disable_irq(int irq) ;

void    init_8259a() ;
void    init_hw_interrupt_idt() ;
void    dummy_irq_handler(int irq) ;
void    set_irq_handler(int irq, irq_handler handler) ;
void    clock_int_handler(int irq) ;


// SOFTWARE
#define SYS_CALL_TBL_CNT    1
#define INT_VECTOR_SYS_CALL 0x90
void    sys_call() ;
int     sys_get_ticks() ;

void    init_sw_interrupt_idt() ;

#endif
