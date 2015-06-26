/**
    Provide by Walon Li
    2015/6/15
**/

#include "kernel/interrupt.h"
#include "kernel/io.h"
#include "kernel/global.h"
#include "kernel/console.h"
#include "lib/debug.h"

static CONSOLE console[CONSOLE_CNT] ;
static uint8_t current_console = 0 ;


static void set_cursor(uint32_t pos)
{
    __asm__ volatile("cli");
    outb(CLHR_INDEX, CRTCR_AR) ;
    outb((pos>>8) & 0xff, CRTCR_DR) ;
    outb(CLLR_INDEX, CRTCR_AR) ;
    outb((pos & 0xff), CRTCR_DR) ;
    __asm__ volatile("sti");
}

static void set_start_address(uint32_t addr)
{
    __asm__ volatile("cli");
    outb(SAHR_INDEX, CRTCR_AR) ;
    outb((addr>>8) & 0xff, CRTCR_DR) ;
    outb(SALR_INDEX, CRTCR_AR) ;
    outb(addr & 0xff, CRTCR_DR) ;
    __asm__ volatile("sti");
}

static void scroll_console(CONSOLE *con, int direction)
{
    if (direction > 0)
    {
        if (con->vga_start_addr > con->vga_mem_start)
            con->vga_start_addr -= (direction * TEXT_MODE_WIDTH) ;
    }
    else if (direction < 0)
    {
        if ((con->vga_start_addr + TEXT_MODE_SIZE) < con->vga_mem_end)
            con->vga_start_addr -= (direction * TEXT_MODE_WIDTH) ;
    }

    if (&console[current_console] == con)
    {
        set_start_address(con->vga_start_addr) ;
        set_cursor(con->cursor_pos) ;
    }
}

static void print_character(CONSOLE *con, char c)
{
    uint16_t *vmem_ptr = (uint16_t*)(VGA_MEM_BASE + (con->cursor_pos * 2)) ;

    switch (c)
    {
    case '\n' :
        if (con->cursor_pos < con->vga_mem_end - TEXT_MODE_WIDTH)
        {
            con->cursor_pos = con->vga_mem_start
                              + (TEXT_MODE_WIDTH * ((con->cursor_pos - con->vga_mem_start)
                                                   / TEXT_MODE_WIDTH + 1)) ;
        }
        break ;
    case '\b' :
        if (con->cursor_pos > con->vga_mem_start)
        {
            con->cursor_pos-- ;
            *(vmem_ptr-1) = ' ' | (0x0f << 8) ;
        }
        break ;
    default:
        if (con->cursor_pos < con->vga_mem_end - 1)
        {
            *vmem_ptr = c | (0x0f << 8) ; // white character
            con->cursor_pos++ ;
        }
    }

    while (con->cursor_pos >= con->vga_start_addr + TEXT_MODE_SIZE)
        scroll_console(con, -1) ;

    if (&console[current_console] == con)
    {
        set_cursor(con->cursor_pos) ;
        set_start_address(con->vga_start_addr) ;
    }
}

static void init_consoles()
{
    for (int i = 0 ; i < CONSOLE_CNT ; ++i)
    {
        // init input buffer
        console[i].buf_count = 0 ;
        console[i].buf_head = console[i].buf_tail = console[i].in_buf ;

        // init console vga
        uint32_t vmem_size = VGA_MEM_SIZE >> 1 ; // word
        uint32_t offset = vmem_size / CONSOLE_CNT ;

        console[i].vga_mem_start = i * offset ;
        console[i].vga_mem_end = console[i].vga_mem_start + offset ;
        console[i].vga_start_addr = console[i].vga_mem_start ;

        console[i].cursor_pos = console[i].vga_mem_start ;

        // set cursor to begin
        if (i == 0)
        {
            // con1, inherit position
            console[i].cursor_pos = position / 2;
            position = 0 ;
        }
        else
        {
            print_character(&console[i], i+'0') ;
            print_character(&console[i], '#') ;
            print_character(&console[i], ' ') ;
        }
    }
}

static void change_current_console(uint8_t i)
{
    if (i < 0 || i >= CONSOLE_CNT) return ;

    current_console = i ;
    set_cursor(console[i].cursor_pos) ;
    set_start_address(console[i].vga_start_addr) ;
}

static void console_read_key(CONSOLE *con)
{
    read_keyboard(con) ;
}

static void console_write_key(CONSOLE *con)
{

    if (con->buf_count)
    {
        char c = *(con->buf_tail++) ;
        if (con->buf_tail == con->in_buf + CONSOLE_BUF_LENGTH)
            con->buf_tail = con->in_buf ;

        con->buf_count-- ;
        print_character(con, c) ;
    }
}

static bool store_key(CONSOLE *con, uint32_t key)
{
    if (con->buf_count < CONSOLE_BUF_LENGTH)
    {
        *(con->buf_head++) = key ;
        if (con->buf_head == con->in_buf+CONSOLE_BUF_LENGTH)
            con->buf_head = con->in_buf ; // reset
        con->buf_count++ ;
        return true ;
    }
    return false ;
}

// public function
void console_task()
{
    init_keyboard() ;
    init_consoles() ;

    change_current_console(0) ;

    while(true)
    {
        for (int i = 0 ; i < CONSOLE_CNT ; ++i)
        {
            if (i == current_console)
                console_read_key(&console[i]) ;
            console_write_key(&console[i]) ;
        }
    }
}

void store_key_into_console(CONSOLE *con, uint32_t key)
{
    // if FLAG_EXT is set, don't show this character
    if (!(key & FLAG_EXT))
    {
        if (store_key(con, key))
        {
            // store key success
        }
        else
        {
            // buffer is filled.
        }
    }
    else
    {
        uint32_t raw_code = key & MASK_RAW ;

        // try to implement move one line ;
        switch (raw_code)
        {
        case ENTER:
            store_key(con, '\n') ;
            break ;
        case BACKSPACE:
            store_key(con, '\b') ;
            break ;
        case UP:
            // shift + up arrow
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
                scroll_console(con, 1) ;
            break ;
        case DOWN:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
                scroll_console(con, -1) ;
            break ;
        case F1:
        case F2:
        case F3:
            if ((key & FLAG_CTRL_L) || (key & FLAG_CTRL_R))
            {
                change_current_console(raw_code - F1) ;
            }
            break ;
        default:
            break ;
        }
    }
}

void console_write(int con_id, char *buf, int len)
{
    char *p = buf;
    int i = len ;
    while(i--)
    {
        print_character(&console[con_id], *p++) ;
    }
}

