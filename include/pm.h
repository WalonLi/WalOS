/**
    Provide by Walon Li
    2015/5/22
**/

#ifndef WALOS_PM_H
#define WALOS_PM_H

#include "type.h"
#include "pm.h"

#define GDT_SIZE    128
#define	IDT_SIZE	256


typedef struct _DESCRIPTOR
{
    uint16  limit_low ;
    uint16  base_low ;
    uint8   base_mid ;
    uint8   attr1 ; //P:1 DPL:2 DT:1 TYPE:4
    uint8   limit_hight_attr2 ; // G:1 D:1 L:1 AVL:1 SegLimitHight:4
    uint8   base_hight ;
} DESCRIPTOR ;



uint8		gdt_ptr[6];	// 0-15:Limit  16-47:Base 
DESCRIPTOR	gdt[GDT_SIZE];

/*
u8		idt_ptr[6];	// 0-15:Limit  16-47:Base
GATE		idt[IDT_SIZE];
*/


#endif
