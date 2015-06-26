/**
    Provide by Walon Li
    2015/6/25
**/

#include "lib/debug.h"
#include "lib/common.h"
#include "type.h"
void assert_failure(char *exp, char *file, char *base_file, int line)
{
    printf("%c assert(%s) %s %s %d", MAGIC_CH_ASSERT, exp, file, base_file, line) ;
    while (1) ;

    __asm__ __volatile__("ud2") ; // carefully code.
}

void CRITICAL(const char *fmt, ...)
{
    char buf[256] = {0} ;
    va_list arg = (va_list)((char*)&fmt + 4) ;

    vsprintf(buf, fmt, arg) ;
    printf("%c critical %s", MAGIC_CH_CRITICAL, buf) ;

    __asm__ __volatile__("ud2");
}

