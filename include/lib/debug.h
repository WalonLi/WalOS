/**
    Provide by Walon Li
    2015/6/25
**/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_MODE
#include "lib/common.h"


#ifdef DEBUG_MODE
void assert_failure(char *exp, char *file, char *base_file, int line) ;
#define ASSERT(exp) if(exp) ; \
                    else assert_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

void CRITICAL(const char *fmt, ...) ;

#else
#define ASSERT(exp)
#define CRITICAL(exp)
#endif

#define MAGIC_CH_CRITICAL   '\002'
#define MAGIC_CH_ASSERT     '\003'


#endif
