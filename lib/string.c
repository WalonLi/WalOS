/**
    Provide by Walon Li
    2015/5/27
**/

#include "type.h"

void *memcpy(void *dest, const void *src, uint64_t cnt)
{
    const char *src_c = (const char*)src ;
    char *dest_c = (char*)dest ;
    
    while ( cnt-- ) 
        *dest_c++ = *src_c++ ;
    
    return dest ;
}

void *memset(void *src, char c, uint64_t cnt)
{
    char *src_c = (char*)src ;
    
    for ( uint64_t i = 0 ; i < cnt ; ++i ) 
        src_c[i] = c ;
    
    return src ;
}

int memcmp(const void *s1, const void *s2, uint64_t cnt)
{
    const char *s1_c = (const char*)s1 ;
    const char *s2_c = (const char*)s2 ;
    int result = 0 ;
    while ( cnt-- )
    {
        result = (*s1_c++) - (*s2_c++) ;
        if (result) return result ;
    }
    
    return 0 ;
}



