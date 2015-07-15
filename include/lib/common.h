/**
    Provide by Walon Li
    2015/5/23
**/

#ifndef __COMMON_H__
#define __COMMON_H__

#include "type.h"

// walos lib
void show_msg(char *msg) ;
void show_msg_ext(char *msg, uint16_t color) ;

// standard lib
int atoi(const char *s) ;
char *itoa(int value, char *dest) ;
char *itoa_base(int value, char *dest, int radix) ;

int strlen(const char *buf) ;

//void delay(int time) ;
void delay(int milli) ;

int printf(const char *fmt, ...) ;
int vsprintf(char *buf, const char *fmt, va_list args) ;

void read_port(uint16_t port, void *buf, int length) ;

#endif
