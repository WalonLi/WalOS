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
    init_8259_irq() ;
    
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
    show_msg_ext("Exception : ", 0x74) ;
    show_msg_ext(err_msg[vector], 0x74) ;
    show_msg("\n") ;
    
    char buffer[80] = { 0 };
    
    show_msg_ext("eflags:", 0x74) ;
    show_msg_ext(itoa_base(eflags, buffer, 16), 0x74) ;
    
    show_msg_ext(" cs:", 0x74) ;
    show_msg_ext(itoa_base(cs, buffer, 16), 0x74) ;
    
    show_msg_ext(" eip:", 0x74) ;
    show_msg_ext(itoa_base(eip, buffer, 16), 0x74) ;
    
    show_msg_ext(" err_code:", 0x74) ;
    show_msg_ext(itoa_base(err_code, buffer, 16), 0x74) ;        
    
}

void hw_irq_handler(int irq)
{
    char str[80] = {0} ;
    show_msg_ext("irq:", 0x74) ;
    show_msg_ext(itoa_base(irq, str, 16), 0x74) ;
    show_msg("\n") ;
}

void init_idt_desc(unsigned char vector, uint8_t type, interrupt_handler handler, unsigned char privilege)
{
    // GATE *gate_ptr = &idt[vector] ;
    uint32_t base = (uint32_t)handler ;

    idt[vector].offset_low = base & 0xffff ;
    idt[vector].selector = SELECTOR_KERNEL_CS ;
    idt[vector].d_count = 0 ;
    idt[vector].attr = type | (privilege<<5) ;
    idt[vector].offset_hight = (base>>16) & 0xffff ;
}

void init_idt_descs()
{
    // initial all interrupt gate
    init_idt_desc(HW_INT_VECTOR_DIVISION_BY_ERROR, DA_386IGate, _division_by_error, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_DEBUGGER, DA_386IGate, _debugger, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_NMI, DA_386IGate, _nmi, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_BREAKPOINT, DA_386IGate, _breakpoint, PRI_USER) ;
    init_idt_desc(HW_INT_VECTOR_OVERFLOW, DA_386IGate, _overflow, PRI_USER) ;
    init_idt_desc(HW_INT_VECTOR_BOUNDS, DA_386IGate, _bounds, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_INVALILD_OPCODE, DA_386IGate, _invalid_opcode, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_COPR_NOT_AVAIL, DA_386IGate, _coprocessor_not_available, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_DOUBLE_FAULT, DA_386IGate, _double_foult, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_COPR_SEG_OVERRUN, DA_386IGate, _coprocessor_segment_overrun, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_INVALID_TSS, DA_386IGate, _invalid_task_state_segment, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_SEG_NOT_PRESENT, DA_386IGate, _segment_not_present, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_STACK_FAULT, DA_386IGate, _stack_fault, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_GERNERAL_PROTECT_FAULT, DA_386IGate, _general_protected_fault, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_PAGE_FAULT, DA_386IGate, _page_fault, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_RESERVED, DA_386IGate, _reserved, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_MATH_FAULT, DA_386IGate, _math_fault, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_ALIGN_CHECK, DA_386IGate, _alignment_check, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_MACHINE_CHECK, DA_386IGate, _machine_check, PRI_KRNL) ;
    init_idt_desc(HW_INT_VECTOR_SIMD_FLOAT_POINT_EXECPT, DA_386IGate, _simd_floating_point_exception, PRI_KRNL) ;

}












