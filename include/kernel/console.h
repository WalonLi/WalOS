/**
    Provide by Walon Li
    2015/6/17
**/

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "type.h"
#include "kernel/keyboard.h"
#include "kernel/vga.h"

#define CONSOLE_BUF_LENGTH  128
#define CONSOLE_CNT         3

typedef struct _CONSOLE
{
    uint32_t in_buf[CONSOLE_BUF_LENGTH] ;
    char *buf_head ;
    char *buf_tail ;
    int buf_count ;

    uint32_t cursor_pos ;
    uint32_t vga_start_addr ;
    uint32_t vga_mem_start ;    // once set, doesn't change anymore.
    uint32_t vga_mem_end ;      // once set, doesn't change anymore.
} CONSOLE ;

void console_task() ;
void store_key_into_console(CONSOLE con, uint32_t key) ;
#endif // __CONSOLE_H__
