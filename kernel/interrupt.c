/**
    Provide by Walon Li
    2015/6/8
**/

#include "kernel/io.h"
#include "kernel/interrupt.h"
#include "kernel/global.h"
#include "lib/common.h"

/****************************************************************/
/****************************************************************/
/**************** HARDWARE interrupt essential*******************/
/****************************************************************/
/****************************************************************/
void init_8259a()
{
    // ICW1
    outb(0x11, INT_M_CTRL) ;
    outb(0x11, INT_S_CTRL) ;

    // ICW2
    outb(INT_VECTOR_IRQ0, INT_M_CTRL_MASK) ; // master entry is 0x20
    outb(INT_VECTOR_IRQ8, INT_S_CTRL_MASK) ; // slaver entry is 0x28

    // ICW3
    outb(0x4, INT_M_CTRL_MASK) ; // master 2 = slaver 4
    outb(0x2, INT_S_CTRL_MASK) ;

    // ICW4
    outb(0x1, INT_M_CTRL_MASK) ;
    outb(0x1, INT_S_CTRL_MASK) ;

    // OCW1
    outb(0xff, INT_M_CTRL_MASK) ;
    outb(0xff, INT_S_CTRL_MASK) ;

    // reset IRQ handler
    for ( int i = 0 ; i < IRQ_CNT ; i++ )
    {
        irq_table[i] = (irq_handler)dummy_irq_handler ;
    }
}

// PIT Programmable Interval timer
void init_8253_pit()
{
    // because bochs does't have realtime clock, so it's invalid.
    outb(TIMER_RATE_GEN, TIMER_MODE) ;
    outb((uint8_t)(TIMER_FREQ/TIMER_HZ), TIMER0) ;
    outb((uint8_t)((TIMER_FREQ/TIMER_HZ) >> 8), TIMER0) ;

    // fuck register... outb(value, port)
//    outb(TIMER_MODE, TIMER_RATE_GEN) ;
//    outb(TIMER0, (uint8_t)(TIMER_FREQ/TIMER_HZ)) ;
//    outb(TIMER0, (uint8_t)((TIMER_FREQ/TIMER_HZ) >> 8)) ;
}

void init_hw_interrupt_idt()
{
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hw_int00, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hw_int01, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hw_int02, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hw_int03, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hw_int04, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hw_int05, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hw_int06, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hw_int07, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hw_int08, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hw_int09, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hw_int10, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hw_int11, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hw_int12, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hw_int13, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hw_int14, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hw_int15, PRI_KRNL) ;
}

void dummy_irq_handler(int irq)
{
    char buf[50] = {0};
    show_msg("irq:") ;
    show_msg(itoa(irq, buf)) ;
    show_msg("\n") ;
}

void set_irq_handler(int irq, irq_handler handler)
{
    disable_irq(irq) ;
    irq_table[irq] = handler ;
}


// clock interrupt
void clock_int_handler(int irq)
{
    //show_msg("#") ;
    ticks++ ;
    process_ready->ticks-- ;

    if (int_reenter != 0)
    {
        //char buf[10] = {0} ;
        //show_msg(itoa(int_reenter, buf)) ;
        return  ;
    }

    if (process_ready->ticks > 0)
    {
        return ;
    }

    process_schedule() ;
}


/****************************************************************/
/****************************************************************/
/**************** SOFTWARE interrupt essential*******************/
/****************************************************************/
/****************************************************************/
void init_sw_interrupt_idt()
{
    init_idt_desc(INT_VECTOR_SYS_CALL, DA_386IGate, sys_call, PRI_USER) ;
}

int sys_get_ticks()
{
    return ticks ;
}




