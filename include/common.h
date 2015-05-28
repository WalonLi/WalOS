/**
    Provide by Walon Li
    2015/5/23
**/

#ifndef COMMON_H
#define COMMON_H

#include "type.h"

// walos lib
void show_msg(char *msg) ;
void show_msg_ext(char *msg, uint16_t color) ;

// standard lib
int atoi(const char *s) ;
char *itoa(int value, char *dest) ;
char *itoa_base(int value, char *dest, int radix) ;


#endif
