/**
    Provide by Walon Li
    2015/5/27
**/

#include "type.h"
#include "common.h"
#include "string.h"

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


int atoi(const char *s)
{
    bool positive = true ;
    int num = 0 ;

    if (*s == '-')
    {
        positive = false ;
        s++ ;
    }
    else if (*s == '+')
    {
        s++ ;
    }

    while (*s)
    {
        num = num*10 + (*s - '0') ;
        s++ ;
    }

    return (positive) ? num : (num | 0x80000000) ;
}

void reverse(char *s)
{
    for (int i = 0, j = strlen(s)-1 ; i < j ; i++, j--)
    {
        int t = s[i] ;
        s[i] = s[j] ;
        s[j] = t ;
    }
}

char *itoa(int value, char *dest)
{
    bool positive = true ;

    if (value < 0)
    {
        value = -value ;
        positive = false ;
    }

    int i = 0 ;
    while (value)
    {
        dest[i++] = value % 10 + '0' ;
        value /= 10 ;
    }

    if (!positive) dest[i++] = '-' ;
    dest[i] = '\0' ;

    reverse(dest) ;

    return dest ;
}

