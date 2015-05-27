/**
    Provide by Walon Li
    2015/5/27
**/

#include "type.h"

extern uint32_t position ;

void show_msg(char *msg)
{
    for ( char *c = msg ; *c != '\0' ; ++c )
    {
        if (*c == '\n')
            position = ((position / 160) + 1) * 160 ;
        else
        {
            uint16_t value = 0xf00 | *c ;
            __asm__ volatile("movw %0, %%gs:(%%edi)": : "a"(value), "D"(position));
            position += 2 ;
        }
    }
} 

