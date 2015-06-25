/**
    Provide by Walon Li
    2015/6/25
**/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "lib/common.h"

#define ASSERT
#ifdef ASSERT
void assert_failure(char *exp, char *file, char *base_file, int line) ;
#define ASSERT(exp) if(!exp) \
                        assert_failure(#esp, __FILE__, __BASE_FILE__, __LINE__) ; \
                    else ;
#else
#define ASSERT(exp)
#endif

#define MAGIC_CH_CRITICAL   '\002'
#define MAGIC_CH_ASSERT     '\003'


#endif
