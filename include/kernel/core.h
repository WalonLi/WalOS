/**
    Provide by Walon Li
    2015/5/22
**/

#ifndef __CORE_H__
#define __CORE_H__

#include "type.h"
//#include "kernel/attribute.h"
//#include "kernel/interrupt.h"
//#include "kernel/process.h"

// This Token to control Bochs or Real machine
#define REAL_MACHINE 1

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


#define GDT_SIZE    128
#define IDT_SIZE    256
#define LDT_SIZE    2


// GDT
// Descriptor index(define in loader)
#define INDEX_DUMMY     0
#define INDEX_FLAT_C    1
#define INDEX_FLAT_RW   2
#define INDEX_VIDEO     3
// add at pm_start.c
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


#define vir2phys(seg_base, vir) (uint32_t)(((uint32_t)seg_base) + (uint32_t)(vir))
uint32_t seg2phys(uint16_t seg) ;

void init_idt_desc(unsigned char vector, uint8_t type, interrupt_handler handler, unsigned char privilege) ;
void init_descriptor(DESCRIPTOR *desc, uint32_t base, uint32_t limit, uint16_t attr) ;

void init_pm_env() ;
void init_ldt_descs() ;
void init_tss() ;
void init_process_main() ;



#endif
