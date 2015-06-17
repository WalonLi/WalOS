/**
    Provide by Walon Li
    2015/6/8
**/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "kernel/core.h"
#include "kernel/process.h"
#include "kernel/interrupt.h"
#include "kernel/keyboard.h"

// global parameter
extern uint8_t      gdt_ptr[6]; // 0-15:Limit  16-47:Base
extern DESCRIPTOR   gdt[GDT_SIZE];

extern uint8_t      idt_ptr[6]; // 0-15:Limit  16-47:Base
extern GATE         idt[IDT_SIZE];

extern uint32_t     position;

extern TSS          tss ;
extern PROCESS      *process_ready;

extern PROCESS      proc_table[TASK_CNT] ;
extern char         task_stack[STACK_SIZE] ;

extern TASK         task_table[TASK_CNT] ;

extern int          int_reenter ;

extern irq_handler  irq_table[IRQ_CNT];

extern void         *sys_call_table[1] ;

extern int          ticks ;

extern uint32_t     key_map[NR_SCAN_CODES * MAP_COLS] ;

#endif
