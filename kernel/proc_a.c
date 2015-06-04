/**
    Provide by Walon Li
    2015/5/30
**/


#include "common.h"

void process_A()
{

    volatile int i = 0 ;
    while (true)
    {
        char buf[10] = { 0 };
        show_msg("A") ;
        show_msg(itoa(i++, buf)) ;
        show_msg(".") ;
        delay(10) ;
    }
}
