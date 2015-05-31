/**
    Provide by Walon Li
    2015/5/30
**/


#include "common.h"

void process_A()
{
    char buf[10] = { 0 };
    int i = 0 ;
    while (true)
    {
        show_msg("A") ;
        show_msg(itoa(i++, buf)) ;
        show_msg(".") ;
        delay(10) ;
    }
}