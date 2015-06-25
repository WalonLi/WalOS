/**
    Provide by Walon Li
    2015/5/27
**/

#include "lib/common.h"
#include "lib/string.h"
#include "kernel/core.h"

extern uint32_t position ;

void show_msg(char *msg)
{
    show_msg_ext(msg, 0) ;
}

void show_msg_ext(char *msg, uint16_t color)
{
    for ( char *c = msg ; *c != '\0' ; ++c )
    {
        if (*c == '\n')
            position = ((position / 160) + 1) * 160 ;
        else
        {
            uint16_t value = (color) ? ((color<<8) | *c) : (0xf00 | *c) ;
            __asm__ volatile("movw %0, %%gs:(%%edi)": : "a"(value), "D"(position));
            position += 2 ;
        }
    }
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *p ;
    va_list next_arg = args ;

    for (p = buf ; *fmt ; fmt++ )
    {
        char tmp[256] = {0};
        char fill_ch ;
        int align = 0 ;

        if (*fmt != '%')
        {
            *p++ = *fmt ;
            continue ;
        }

        fmt++ ;

        // handle %%
        if (*fmt == '%')
        {
            *p++ = *fmt ;
            continue ;
        }
        else if (*fmt == '0')
        {
            fill_ch = '0' ;
            fmt++ ;
        }
        else
            fill_ch = ' ' ;

        for ( ; ((uint8_t)(*fmt) >= '0') && ((uint8_t)(*fmt) <= '9') ; fmt++)
        {
            align *= 10 ;
            align += *fmt - '0' ;
        }

        switch(*fmt)
        {
        case 'x':
            itoa_base(*((int*)next_arg), tmp, 16) ;
            next_arg += 4 ;
            break ;
        case 'd':
            itoa(*((int*)next_arg), tmp) ;
            next_arg += 4 ;
            break ;
        case 'c':
            *tmp = *((char*)next_arg) ;
            next_arg += 4 ;
            break ;
        case 's':
            strcpy(tmp, (*((char**)next_arg))) ;
            next_arg += 4;
            break ;
        default:
            break ;
        }

        if (strlen(tmp) < align)
        {
            for (int j = 0 ; j < align - strlen(tmp) ; j++)
                *p++ = fill_ch ;
        }

        strcpy(p, tmp) ;
        p += strlen(tmp) ;
    }

    *p = '\0' ;
    return (p - buf) ;
}

extern void write(char *buf, int len) ;
int printf(const char *fmt, ...)
{
    int i ;
    char buf[256] ;

    va_list arg = (va_list)((char*)(&fmt) + 4) ;
    i = vsprintf(buf, fmt, arg) ;
    write(buf, i) ; // call system call
    return i ;
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
    return itoa_base(value, dest, 10) ;
}

char *itoa_base(int value, char *dest, int radix)
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
        int c = value % radix ;
        value /= radix ;

        dest[i++] = (c < 10) ? (c+'0') : (c + 'a' - 10) ;
    }

    if (!positive) dest[i++] = '-' ;
    dest[i] = '\0' ;

    reverse(dest) ;

    return dest ;
}

#define HZ 100
int get_ticks() ;

// milli second
void delay(int milli)
{
    int t = get_ticks() ;
    while(((get_ticks()-t) * 1000 / HZ) < milli) ;
    /*
    for (volatile int i = 0 ; i  < time ; i++)
        for (volatile int k = 0 ; k < 10 ; k++)
            for (volatile int j = 0 ; j < 10000 ; j++ ) ;
    */
}




