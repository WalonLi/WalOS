/**
    Provide by Walon Li
    2015/5/31
**/

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "type.h"
#include "kernel/core.h"

typedef void (*task_handler)() ;

typedef struct _REG_STACK
{
    uint32_t gs ;
    uint32_t fs ;
    uint32_t es ;
    uint32_t ds ;
    uint32_t edi ;
    uint32_t esi ;
    uint32_t ebp ;
    uint32_t krnl_esp ;
    uint32_t ebx ;
    uint32_t edx ;
    uint32_t ecx ;
    uint32_t eax ;
    uint32_t ret_addr ;
    uint32_t eip ;
    uint32_t cs ;
    uint32_t eflags ;
    uint32_t esp ;
    uint32_t ss ;
} REG_STACK;

typedef struct _PROCESS
{
    REG_STACK       regs ;
    uint16_t        ldt_sel;
    DESCRIPTOR      ldt[LDT_SIZE] ;
    uint32_t        p_id ;
    char            p_name[16] ;
} PROCESS;

typedef struct _TASK
{
    task_handler    init_eip ;
    int             stack_size ;
    char            name[32] ;
} TASK ;

#define TASK_CNT            3
#define STACK_SIZE_PROC_A   0x8000
#define STACK_SIZE_PROC_B   0x8000
#define STACK_SIZE_PROC_C   0x8000
#define STACK_SIZE          STACK_SIZE_PROC_A + STACK_SIZE_PROC_B + STACK_SIZE_PROC_C


void init_process_main() ;
void restart_process() ;


#endif
