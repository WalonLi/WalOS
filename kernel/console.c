/**
    Provide by Walon Li
    2015/6/15
**/

#include "kernel/interrupt.h"
void console_task()
{
    while(true)
    {
        read_keyboard() ;
    }
}