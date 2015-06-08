/**
    Provide by Walon Li
    2015/6/8
**/
#include "kernel/global.h"
#include "kernel/process.h"
#include "lib/common.h"
#include "lib/string.h"

void init_process_main()
{
    show_msg("kernel main start...\n") ;

    // PROCESS *proc = proc_table ;
    // TASK *task = task_table ;
    char *top_stack = task_stack + STACK_SIZE ;
    uint16_t selector_ldt = SELECTOR_LDT_FIRST ;

    // initial process table
    for ( int i = 0 ; i < TASK_CNT ; i++ )
    {
        proc_table[i].p_id = i ;
        strcpy(proc_table[i].p_name, task_table[i].name) ;

        proc_table[i].ldt_sel = selector_ldt ;

        memcpy(&proc_table[i].ldt[0], &gdt[SELECTOR_KERNEL_CS>>3], sizeof(DESCRIPTOR)) ;
        proc_table[i].ldt[0].attr1 = DA_C | PRI_TASK << 5 ;     // C+RPL1
        memcpy(&proc_table[i].ldt[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(DESCRIPTOR)) ;
        proc_table[i].ldt[1].attr1 = DA_DRW | PRI_TASK << 5 ;   // RW+RPL1

        // ldt selector index = 0
        proc_table[i].regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK ;

        // ldt selector index = 1
        proc_table[i].regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK ;
        proc_table[i].regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK ;
        proc_table[i].regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK ;
        proc_table[i].regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK ;

        proc_table[i].regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK ;

        proc_table[i].regs.eip = (uint32_t)task_table[i].init_eip ;
        proc_table[i].regs.esp = (uint32_t)top_stack ;
        proc_table[i].regs.eflags = 0x1202 ; // IF=1, IOPL=1


        top_stack -= task_table[i].stack_size ;

        // go to next ldt selector
        selector_ldt += (1 << 3) ;
    }


    set_irq_handler(0, clock_int_handler) ;
    enable_irq(0) ;

    //char buf[50] ;
    //show_msg(itoa_base((int)irq_table[0], buf, 16)) ;
    //show_msg("+") ;
    //show_msg(itoa_base((int)clock_int_handler, buf, 16)) ;



    process_ready = proc_table ;
    hw_int_cnt = 0 ;
    ticks = 0 ;
    restart_process() ;
    while(1) ;
}

extern int get_ticks() ;
void process_A()
{
    //volatile int i = 0 ;
    while (true)
    {
        char buf[10] = { 0 };
        show_msg("A") ;
        show_msg(itoa(get_ticks(), buf)) ;
        show_msg(".") ;
        delay(5) ;
    }
}

void process_B()
{
    volatile int i = 0x1000 ;
    while (true)
    {
        char buf[10] = { 0 };
        show_msg("B") ;
        show_msg(itoa(i++, buf)) ;
        show_msg(".") ;
        delay(5) ;
    }
}

void process_C()
{
    volatile int i = 0x2000 ;
    while (true)
    {
        char buf[10] = { 0 };
        show_msg("C") ;
        show_msg(itoa(i++, buf)) ;
        show_msg(".") ;
        delay(5) ;
    }
}
