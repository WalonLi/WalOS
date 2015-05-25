/**
    Provide by Walon Li
    2015/5/22
**/

#ifndef WALOS_PM_H
#define WALOS_PM_H

#include "type.h"

#define GDT_SIZE    128
#define	IDT_SIZE	256


void init_8259A() ;


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


/*
u8		idt_ptr[6];	// 0-15:Limit  16-47:Base
GATE		idt[IDT_SIZE];
*/


#endif
