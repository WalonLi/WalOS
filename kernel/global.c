/**
    Provide by Walon Li
    2015/6/8
**/

#include "kernel/global.h"

// global
uint8_t     gdt_ptr[6]; // 0-15:Limit  16-47:Base
DESCRIPTOR  gdt[GDT_SIZE];

uint8_t     idt_ptr[6]; // 0-15:Limit  16-47:Base
GATE        idt[IDT_SIZE];

uint32_t    position = 0;

TSS         tss ;
PROCESS     *process_ready ;

PROCESS     proc_table[TASK_CNT] ;
char        task_stack[STACK_SIZE] ;

// Task table
extern void process_A() ;
extern void process_B() ;
extern void process_C() ;

TASK        task_table[TASK_CNT] = { {process_A, STACK_SIZE_PROC_A, "P_A"},
                                     {process_B, STACK_SIZE_PROC_B, "P_B"},
                                     {process_C, STACK_SIZE_PROC_C, "P_C"} } ;

//TASK        task_table[TASK_CNT] = { {process_A, STACK_SIZE_PROC_A, "P_A"}} ;
int         int_reenter = 0 ;
irq_handler irq_table[IRQ_CNT];

void        *sys_call_table[SYS_CALL_TBL_CNT] = { sys_get_ticks } ;

int         ticks = 1 ;
