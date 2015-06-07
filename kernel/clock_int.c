/**
    Provide by Walon Li
    2015/6/7
**/

#include "common.h"
#include "process.h"
extern PROCESS *process_ready ;
extern PROCESS proc_table[] ;

void clock_int_handler()
{
    show_msg("#") ;
    process_ready++ ;
    if (process_ready >= proc_table + NR_TASKS)
    {
        process_ready = proc_table ;
    }
}
