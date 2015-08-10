/**
    Provide by Walon Li
    2015/6/8
**/
#include "kernel/global.h"
#include "kernel/process.h"
#include "kernel/message.h"
#include "lib/common.h"
#include "lib/string.h"
#include "lib/debug.h"

void init_process_main()
{
    show_msg("kernel main start...\n") ;

    char *top_stack = task_stack + STACK_SIZE ;
    uint16_t selector_ldt = SELECTOR_LDT_FIRST ;

    // initial process table
    for ( int i = 0 ; i < TOTAL_TASK_CNT ; i++ )
    {
        TASK *task ;
        uint8_t privilege = 0 ;
        uint8_t rpl = 0 ;
        int eflags = 0 ;
        int priority = 0 ;

        if (i < RING0_TASK_CNT)
        {
            // ring0 task
        }
        else if (i < RING0_TASK_CNT + RING1_TASK_CNT)
        {
            // ring1 task (console process)
            task = &r1_task_table[i-RING0_TASK_CNT] ;
            privilege =  PRI_TASK ;
            rpl = RPL_TASK ;
            eflags = 0x1202 ; // IF=1, IOPL=1, bit2 always set 1 ;
            priority = 15 ;
            proc_table[i].console_id = 0 ;
        }
        else if (i < RING0_TASK_CNT + RING1_TASK_CNT + RING2_TASK_CNT)
        {
            // ring2 task
        }
        else
        {
            // ring3 task (user process)
            task = &r3_task_table[i-RING0_TASK_CNT-RING1_TASK_CNT-RING2_TASK_CNT] ;
            privilege =  PRI_USER ;
            rpl = RPL_USER;
            eflags = 0x202 ; // IF=1, bit2 always set 1 ;
            priority = 5 ;
            proc_table[i].console_id = 1 ;
        }


        proc_table[i].id = i ;
        // proc_table[i].console_id = 0 ;

        strcpy(proc_table[i].name, task->name) ;

        proc_table[i].ldt_sel = selector_ldt ;

        memcpy(&proc_table[i].ldt[0], &gdt[SELECTOR_KERNEL_CS>>3], sizeof(DESCRIPTOR)) ;
        proc_table[i].ldt[0].attr1 = DA_C | privilege << 5 ;     // C+RPL1
        memcpy(&proc_table[i].ldt[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(DESCRIPTOR)) ;
        proc_table[i].ldt[1].attr1 = DA_DRW | privilege << 5 ;   // RW+RPL1

        // ldt selector index = 0
        proc_table[i].regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;

        // ldt selector index = 1
        proc_table[i].regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        proc_table[i].regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        proc_table[i].regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        proc_table[i].regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        proc_table[i].regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl ;

        proc_table[i].regs.eip = (uint32_t)task->init_eip ;
        proc_table[i].regs.esp = (uint32_t)top_stack ;
        proc_table[i].regs.eflags = eflags ;

        proc_table[i].ticks = proc_table[i].priority = priority ;

        top_stack -= task->stack_size ;

        // go to next ldt selector
        selector_ldt += (1 << 3) ;
    }


    process_ready = proc_table ;
    int_reenter = 0 ;
    ticks = 0 ;

    init_clock() ;

    restart_process() ;
    while(1) ;
}


void process_schedule()
{
    int biggest_tick = 0 ;

    while(!biggest_tick)
    {
        for (PROCESS *p = proc_table ; p < proc_table+TOTAL_TASK_CNT ; ++p)
        {
            if (p->flags == 0) // no message need transfer
            {
                if (p->ticks > biggest_tick)
                {
                    biggest_tick = p->ticks ;
                    process_ready = p ;
                }
            }
        }

        if (!biggest_tick)
        {
            for (PROCESS *p = proc_table ; p < proc_table + TOTAL_TASK_CNT ;++p)
            {
                if (p->flags == 0) // no message need transfer
                    p->ticks = p->priority ;
            }
        }
    }
}

int ldt_linear_addr(PROCESS *p, int index)
{
    return p->ldt[index].base_high << 24 | p->ldt[index].base_mid << 16 | p->ldt[index].base_low ;
}

void* vir_to_linear(int pid, void* vir_addr)
{
    // in normal case, seg_base = 0, because it occupy 0~4G
    uint32_t seg_base = ldt_linear_addr(&proc_table[pid], INDEX_LDT_RW);
    uint32_t linear_addr = seg_base + (uint32_t)vir_addr;

    if (pid < TOTAL_TASK_CNT) {
        ASSERT(linear_addr == (uint32_t)vir_addr); // should be same, because all process have 0~4G
    }

    return (void*)linear_addr;
}

void deliver_int_to_proc(int n_task)
{
    PROCESS *proc = &proc_table[n_task] ;
    if ((proc->flags & MSG_RECVING)
        && (proc->recv_from == MSG_SOURCE_INTERRUPT || proc->recv_from == P_ANY))
    {
        proc->msg->source = MSG_SOURCE_INTERRUPT ;
        proc->msg->type = MSG_HW_INTERRUPT ;
        proc->msg = null ;
        proc->have_int_msg = false ;
        proc->flags &= ~MSG_RECVING ;
        proc->recv_from = P_NO_TASK ;
        unblock(proc) ;
    }
    else
    {
        proc->have_int_msg = true ;
    }
}

extern int get_ticks() ;
void process_A()
{
    while (true)
    {
        printf("A: ticks:%d ", get_ticks()) ;
        delay(300) ;
    }
}

void process_B()
{
    while (true)
    {
        printf("B:") ;
        delay(300) ;
    }
}

void process_C()
{
    while (true)
    {
        printf("C:") ;
        delay(300) ;
    }
}
