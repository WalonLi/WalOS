/**
    Provide by Walon Li
    2015/6/7
**/

#include "common.h"
#include "process.h"
extern PROCESS *process_ready ;
extern PROCESS proc_table[] ;
extern int hw_int_cnt ;

void clock_int_handler(int irq)
{
    show_msg("#") ;

    if (hw_int_cnt != 0)
    {
        show_msg("!") ;
        return  ;
    }

    process_ready++ ;
    if (process_ready >= proc_table + TASK_CNT)
    {
        process_ready = proc_table ;
    }
}

void fuck(int a)
{
    volatile int i = 0 ;
    if (i < 5)
        ;
}
