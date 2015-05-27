/**
    Provide by Walon Li
    2015/5/27
**/

#include "type.h"

void *memcpy(void *dest, const void *src, uint32_t cnt)
{
    const char *src_c = src ;
    char *dest_c = dest ;
    
    while ( cnt-- ) *dest_c++ = *src_c++ ;
    
    return dest ;
}

void *memset(void *src, char c, uint32_t cnt)
{
    char *src_c = src ;
    
    for ( uint32_t i = 0 ; i < cnt ; ++i ) 
        src_c[i] = c ;
    
    return src ;
}

int memcmp(const void *s1, const void *s2, uint32_t cnt)
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

int strlen(const char *src)
{
    int len = 0 ;
    while (*src++) len++ ;

    return len ;
}

char *strcpy(char *dest, const char *src)
{
    return memcpy(dest, src, strlen(src)) ;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++ ;
        s2++ ;
    }
    return (int)(*s1 - *s2) ;
}
