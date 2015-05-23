/**
    Provide by Walon Li
    2015/5/23
**/

#ifndef COMMON_H
#define COMMON_H

#include "type.h"

void wb_port(uint16_t port, uint8_t value) ;
void ww_port(uint16_t port, uint16_t value) ;
void wd_port(uint16_t port, uint32_t value) ;

uint8_t rb_port(uint16_t port) ;
uint16_t rw_port(uint16_t port) ;
uint32_t rd_port(uint16_t port) ;

void show_msg(char *msg) ;

#endif
