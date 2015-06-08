/**
    Provide by Walon Li
    2015/5/22
**/


#ifndef __STRING_H__
#define __STRING_H__
void memcpy(void *dest, void *src, int size) ;
void memset(void *dest, char c, int cnt) ;
int memcmp(const void *p1, const void *p2, int cnt) ;

int strlen(const char *src) ;
char *strcpy(char *dest, const char *src) ;
int strcmp(const char *p1, const char *p2) ;


#endif
