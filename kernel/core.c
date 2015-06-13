/**
    Provide by Walon Li
    2015/6/8
**/

#include "kernel/core.h"
#include "kernel/global.h"
#include "kernel/exception.h"
#include "lib/string.h"
#include "lib/common.h"


/**************************************************************************/
/*********************** Common lib****************************************/
/**************************************************************************/
// segment to physical address
uint32_t seg2phys(uint16_t seg)
{
    DESCRIPTOR* dest = &gdt[seg >> 3];
    return (dest->base_high<<24 | dest->base_mid<<16 | dest->base_low);
}

// all descriptor initial
void init_idt_desc(unsigned char vector, uint8_t type, interrupt_handler handler, unsigned char privilege)
{
    // GATE *gate_ptr = &idt[vector] ;
    uint32_t base = (uint32_t)handler ;

    idt[vector].offset_low = base & 0xffff ;
    idt[vector].selector = SELECTOR_KERNEL_CS ;
    idt[vector].d_count = 0 ;
    idt[vector].attr = type | (privilege<<5) ;
    idt[vector].offset_high = (base>>16) & 0xffff ;
}

void init_descriptor(DESCRIPTOR *desc, uint32_t base, uint32_t limit, uint16_t attr)
{
    desc->limit_low   = limit & 0x0FFFF;
    desc->base_low    = base & 0x0FFFF;
    desc->base_mid    = (base >> 16) & 0x0FF;
    desc->attr1       = attr & 0xFF;
    desc->limit_high_attr2= (((limit>>16) & 0x0F) | (attr>>8)) & 0xF0;
    desc->base_high   = (base >> 24) & 0x0FF;
}

void init_ldt_descs()
{
    uint16_t selector_ldt = SELECTOR_LDT_FIRST ;
    for ( int i = 0 ; i < TASK_CNT ; i++ )
    {
        init_descriptor(&gdt[selector_ldt>>3],
                      vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc_table[i].ldt),
                      LDT_SIZE * sizeof(DESCRIPTOR)- 1,
                      DA_LDT) ;

        selector_ldt += (1 << 3) ;
    }
}


void init_tss()
{
    memset(&tss, 0, sizeof(tss)) ;
    tss.ss0 = SELECTOR_KERNEL_DS ;
    init_descriptor(&gdt[INDEX_TSS], vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss), sizeof(tss) - 1, DA_386TSS) ;
    tss.iobase = sizeof(tss) ;
}


void init_pm_env()
{
    // adjust position
    for (int x = 0 ; x < 13 ; ++x) show_msg("\n") ;

    show_msg("C code start...\n") ;
    show_msg("Prepare GDT\n") ;

    // show_msg(itoa(a, s)) ;

    // Copy Loader's GDT to new GDT
    memcpy(&gdt,                                // new GDT
           (void*)(*(uint32_t*)(&gdt_ptr[2])),    // base of old GDT
           *((uint16_t*)(&gdt_ptr[0])) + 1 ) ;    // limit of old GDT


    uint16_t *gdt_limit = (uint16_t*)(&gdt_ptr[0]) ;
    uint32_t *gdt_base = (uint32_t*)(&gdt_ptr[2]) ;


    // set new limit(GDT 128)
    *gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1 ;
    // set new base
    *gdt_base = (uint32_t)&gdt ;

    show_msg("Prepare IDT\n") ;
    // prepare IDT
    uint16_t *idt_limit = (uint16_t*)(&idt_ptr[0]) ;
    uint32_t *idt_base = (uint32_t*)(&idt_ptr[2]);
    *idt_limit = IDT_SIZE * sizeof(GATE) - 1 ;
    *idt_base = (uint32_t)&idt ;

    init_8259a() ;
    init_hw_exception_idt() ;
    init_hw_interrupt_idt() ;
    init_sw_interrupt_idt() ;
    init_ldt_descs() ;
    init_tss() ;


    show_msg("C code end...\n") ;
}






