/**
    Provide by Walon Li
    2015/6/25
**/

#include "lib/debug.h"

void assert_failure(char *exp, char *file, char *base_file, int line)
{
    printf("%c assert(%s) %s %s %d", MAGIC_CH_ASSERT, exp, file, base_file, line) ;
    while (1) ;

    __asm__ __volatile__("ud2") ; // carefully code.
}



