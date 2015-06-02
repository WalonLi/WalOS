/**
    Provide by Walon Li
    2015/5/23
**/

#include "io.h"
#include "pm.h"

// master 8259a
#define INT_M_CTRL  0x20
#define INT_M_CTRL_MASK     0x21
// slaver 8259a
#define INT_S_CTRL  0xA0
#define INT_S_CTRL_MASK     0xA1

#define INT_VECTOR_IRQ0     0x20
#define INT_VECTOR_IRQ8     0x28

void init_8259A()
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
    outb(0xfe, INT_M_CTRL_MASK) ;
    outb(0xff, INT_S_CTRL_MASK) ;
}


void init_8259_irq()
{
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, _hw_int00, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, _hw_int01, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, _hw_int02, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, _hw_int03, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, _hw_int04, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, _hw_int05, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, _hw_int06, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, _hw_int07, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, _hw_int08, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, _hw_int09, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, _hw_int10, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, _hw_int11, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, _hw_int12, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, _hw_int13, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, _hw_int14, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, _hw_int15, PRI_KRNL) ;
}

