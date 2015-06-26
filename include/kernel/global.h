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
extern uint8_t      gdt_ptr[]; // 0-15:Limit  16-47:Base
extern DESCRIPTOR   gdt[];

extern uint8_t      idt_ptr[]; // 0-15:Limit  16-47:Base
extern GATE         idt[];

extern uint32_t     position;

extern TSS          tss ;
extern PROCESS      *process_ready;

extern PROCESS      proc_table[] ;
extern char         task_stack[] ;

extern TASK         r1_task_table[] ;
extern TASK         r3_task_table[] ;

extern int          int_reenter ;

extern irq_handler  irq_table[];

extern void         *sys_call_table[] ;

extern int          ticks ;

extern uint32_t     key_map[] ;

#endif
