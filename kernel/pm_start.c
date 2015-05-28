/**
    Provide by Walon Li
    2015/5/22
**/

#include "type.h"
#include "pm.h"
#include "string.h"
#include "common.h"


uint8_t     gdt_ptr[6]; // 0-15:Limit  16-47:Base 
DESCRIPTOR  gdt[GDT_SIZE];

uint8_t     idt_ptr[6]; // 0-15:Limit  16-47:Base 
GATE        idt[IDT_SIZE];

uint32_t    position = 0;

void cstart()
{
    // adjust position
    // position = 0 ;
    // char s [100] ={ 0 } ;
    // int a = 5 ;
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
    uint32_t *idt_base = (uint32_t*)(&idt_ptr[2]) ;
    *idt_limit = IDT_SIZE * sizeof(GATE) - 1 ;
    *idt_base = (uint32_t)&idt ;  
    
    init_8259A() ;
    init_idt_descs() ;
    
    show_msg("C code end...\n") ;

}

void hw_exception_handler(int vector, int err_code, int eip, int cs, int eflags)
{
    // uint16_t color = 0x74 ;
    char *err_msg[] = {"##_division_by_error",
                       "##_debugger",
                       "##_nmi",
                       "##_breakpoint",
                       "##_overflow",
                       "##_bounds",
                       "##_invalid_opcode",
                       "##_coprocessor_not_available",
                       "##_double_foult",
                       "##_coprocessor_segment_overrun",
                       "##_invalid_task_state_segment",
                       "##_segment_not_present",
                       "##_stack_fault",
                       "##_general_protected_fault",
                       "##_page_fault",
                       "##_reserved",
                       "##_math_fault",
                       "##_alignment_check",
                       "##_machine_check",
                       "##_simd_floating_point_exception"} ;
    position = 0 ;
    show_msg("Exception : ", 0x74) ;
    show_msg(err_msg[vector], 0x74) ;
    show_msg("\n") ;
    
    char buffer[80] = { 0 };
    
    show_msg("eflags:", 0x74) ;
    show_msg(itoa(eflags, buffer, 16)) ;
    
    show_msg(" cs:", 0x74) ;
    show_msg(itoa(cs, buffer, 16)) ;
    
    show_msg(" eip:", 0x74) ;
    show_msg(itoa(eip, buffer, 16)) ;
    
    show_msg(" err_code:", 0x74) ;
    show_msg(itoa(err_code, buffer, 16)) ;        
    
}

void init_idt_desc(unsigned char vector, uint8_t type, interrupt_handler handler, unsigned char privilege)
{
    // GATE *gate_ptr = &idt[vector] ;
    uint32_t base = (uint32_t)handler ;
        uint16_t  offset_low ;
    uint16_t  selector ;
    uint8_t   d_count ;
    uint8_t   attr ; //P:1 DPL:2 DT:1 TYPE:4
    uint16_t  offset_hight ;
    idt[vector].offset_low = base & 0xffff ;
    idt[vector].selector = SELECTOR_KERNEL_CS ;
    idt[vector].d_count = 0 ;
    idt[vector].attr = type | (privilege<<5) ;
    idt[vector].offset_hight = (base>>16) & 0xffff ;
}

void init_idt_descs()
{
    // initial all interrupt gate
    // init_idt_desc() ;
}












