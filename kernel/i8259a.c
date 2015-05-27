/**
    Provide by Walon Li
    2015/5/23
**/

#include "io.h"

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
    outb(INT_M_CTRL, 0x11) ;
    outb(INT_S_CTRL, 0x11) ;

    // ICW2
    outb(INT_M_CTRL_MASK, INT_VECTOR_IRQ0) ; // master entry is 0x20
    outb(INT_S_CTRL_MASK, INT_VECTOR_IRQ8) ; // slaver entry is 0x28

    // ICW3
    outb(INT_M_CTRL_MASK, 0x4) ; // master 2 = slaver 4
    outb(INT_S_CTRL_MASK, 0x2) ;

    // ICW4
    outb(INT_M_CTRL_MASK, 0x1) ;
    outb(INT_S_CTRL_MASK, 0x1) ;

    // OCW1
    outb(INT_M_CTRL_MASK, 0xf) ;
    outb(INT_S_CTRL_MASK, 0xfd) ;
}

