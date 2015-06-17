/**
    Provide by Walon Li
    2015/6/15
**/

#include "kernel/interrupt.h"
#include "kernel/io.h"
#include "kernel/global.h"
#include "kernel/console.h"

static CONSOLE console[CONSOLE_CNT] ;
static uint8_t current_console = 0 ;

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

        // set cursor to begin
        if (i == 0)
        {
            // con1, inherit position
            console[i].cursor_pos = position / 2 ;
            position = 0 ;
        }
        else
        {
            print_character(console[i], i+'0') ;
            print_character(console[i], '#') ;
        }
    }
}

static void set_cursor(uint32_t pos)
{
    __asm__ volatile("cli");
    outb(CLHR_INDEX, CRTCR_AR) ;
    outb(((pos/2)>>8) & 0xff, CRTCR_DR) ;
    outb(CLLR_INDEX, CRTCR_AR) ;
    outb((pos/2) & 0xff, CRTCR_DR) ;
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

static void print_character(CONSOLE con, char c)
{
    uint16_t *vmem_ptr = (uint16_t*)(VGA_MEM_BASE + (con.cursor_pos * 2)) ;
    *vmem_ptr = (ch << 8) | 0x07 ; // white character
    con.cursor_pos++ ;

    set_cursor(con.cursor_pos) ;
}

static void change_current_console(uint8_t i)
{
    if (i < 0 || i >= CONSOLE_CNT) return ;

    current_console = i ;
    set_cursor(console[i].cursor_pos) ;
    set_start_address(console[i].vga_start_addr) ;
}

static void console_read_key(CONSOLE con)
{
    read_keyboard(con) ;
}

static void console_write_key(CONSOLE con)
{
    if (con.in_buf)
    {
        char c = *(con.buf_tail++) ;
        if (con.buf_tail == con.in_buf + CONSOLE_BUF_LENGTH)
            con.buf_tail = con.in_buf ;

        con.buf_count-- ;
        print_character(con, c) ;
    }
}

static void scroll_console(CONSOLE con, int direction)
{
    if (direction > 0)
        if (con.vga_start_addr > con.vga_mem_start)
            con.vga_start_addr -= (direction * TEXT_MODE_WIDTH)
    else if (direction < 0)
        if ((con.vga_start_addr + TEXT_MODE_SIZE) < con.vga_mem_end)
            con.vga_start_addr += (direction * TEXT_MODE_WIDTH)

    set_start_address(con.vga_start_addr) ;
    set_cursor(con.cursor_pos) ;
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
                console_read_key(console[i]) ;
            console_write_key(console[i]) ;
        }
    }
}

void store_key_into_console(CONSOLE con, uint32_t key)
{
    // if FLAG_EXT is set, don't show this character
    if (!(k & FLAG_EXT))
    {
        if (con.buf_count < CONSOLE_BUF_LENGTH)
        {
            *(con.buf_head++) = key ;
            if (con.buf_head == con.in_buf+CONSOLE_BUF_LENGTH)
                con.buf_head = con.in_buf ; // reset
            con.buf_count++ ;
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
            if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R))
            {
                change_current_console(raw_code - F1) ;
            }
            break ;
        default:
            break ;
        }
    }
}

