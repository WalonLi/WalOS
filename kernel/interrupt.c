/**
    Provide by Walon Li
    2015/6/8
**/

#include "kernel/io.h"
#include "kernel/interrupt.h"
#include "kernel/global.h"
#include "kernel/keyboard.h"
#include "kernel/vga.h"
#include "kernel/console.h"
#include "kernel/process.h"
#include "lib/common.h"
#include "lib/string.h"
#include "lib/debug.h"

static KB_INPUT kb_in ;

static bool num_lock ;
static bool caps_lock ;
static bool scroll_lock ;
/****************************************************************/
/****************************************************************/
/**************** HARDWARE interrupt essential*******************/
/****************************************************************/
/****************************************************************/
void init_8259a()
{
    // ICW1
    outb(0x11, INT_M_CTRL) ;
    outb(0x11, INT_S_CTRL) ;

    // ICW2
    outb(INT_VECTOR_IRQ0, INT_M_CTRL_MASK) ; // master entry is 0x20
    outb(INT_VECTOR_IRQ8, INT_S_CTRL_MASK) ; // slaver entry is 0x28

    // ICW3
    outb(0x4, INT_M_CTRL_MASK) ; // master 2 = slaver 4
    outb(0x2, INT_S_CTRL_MASK) ;

    // ICW4
    outb(0x1, INT_M_CTRL_MASK) ;
    outb(0x1, INT_S_CTRL_MASK) ;

    // OCW1
    outb(0xff, INT_M_CTRL_MASK) ;
    outb(0xff, INT_S_CTRL_MASK) ;

    // reset IRQ handler
    for ( int i = 0 ; i < IRQ_CNT ; i++ )
    {
        irq_table[i] = (irq_handler)dummy_irq_handler ;
    }
}

// PIT Programmable Interval timer
void init_8253_pit()
{
    // because bochs does't have realtime clock, so it's invalid.
    outb(TIMER_RATE_GEN, TIMER_MODE) ;
    outb((uint8_t)(TIMER_FREQ/TIMER_HZ), TIMER0) ;
    outb((uint8_t)((TIMER_FREQ/TIMER_HZ) >> 8), TIMER0) ;

    // fuck register... outb(value, port)
//    outb(TIMER_MODE, TIMER_RATE_GEN) ;
//    outb(TIMER0, (uint8_t)(TIMER_FREQ/TIMER_HZ)) ;
//    outb(TIMER0, (uint8_t)((TIMER_FREQ/TIMER_HZ) >> 8)) ;
}

void init_hw_interrupt_idt()
{
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hw_int00, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hw_int01, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hw_int02, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hw_int03, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hw_int04, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hw_int05, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hw_int06, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hw_int07, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hw_int08, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hw_int09, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hw_int10, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hw_int11, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hw_int12, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hw_int13, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hw_int14, PRI_KRNL) ;
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hw_int15, PRI_KRNL) ;
}

void dummy_irq_handler(int irq)
{
    char buf[50] = {0};
    show_msg("irq:") ;
    show_msg(itoa(irq, buf)) ;
    show_msg("\n") ;
}

void set_irq_handler(int irq, irq_handler handler)
{
    disable_irq(irq) ;
    irq_table[irq] = handler ;
}


void init_clock()
{
    init_8253_pit() ; // adjust timer to 10 HZ

    set_irq_handler(IRQ_CLOCK, clock_int_handler) ;
    enable_irq(IRQ_CLOCK) ;
}

// clock interrupt
void clock_int_handler(int irq)
{
    //show_msg("#") ;
    ticks++ ;
    process_ready->ticks-- ;

    if (int_reenter != 0)
    {
        //char buf[10] = {0} ;
        //show_msg(itoa(int_reenter, buf)) ;
        return  ;
    }

    if (process_ready->ticks > 0)
    {
        return ;
    }

    process_schedule() ;
}

static void kb_wait()
{
    // check buffer is clean
    uint8_t state ;
    do
    {
        state = inb(KB_CMD) ;
    } while (state & 0x02) ;
}

static void kb_ack()
{
    uint8_t read ;
    do
    {
        read = inb(KB_DATA) ;
    } while (read != KB_ACK) ;
}

static void set_kb_led()
{
    uint8_t led = (caps_lock<<2) | (num_lock<<1) | scroll_lock ;

    kb_wait() ;
    outb(LED_CODE, KB_DATA) ;
    kb_ack() ;

    kb_wait() ;
    outb(led, KB_DATA) ;
    kb_ack() ;
}

void init_keyboard()
{
    kb_in.count = 0 ;
    kb_in.head = kb_in.tail = kb_in.buf ;

    num_lock = true ;
    caps_lock = false ;
    scroll_lock = false ;
    set_kb_led() ;

    set_irq_handler(IRQ_KEYBOARD, keyboard_int_handler) ;
    enable_irq(IRQ_KEYBOARD) ;
}

// keyboard interrupt
void keyboard_int_handler(int irq)
{
    uint8_t scan_code = inb(0x60) ;

    if (kb_in.count < KB_IN_BUF_LENGTH)
    {
        *(kb_in.head++) = scan_code ;

        if (kb_in.head == kb_in.buf + KB_IN_BUF_LENGTH)
        {
            kb_in.head = kb_in.buf ; // reset
        }
        kb_in.count++ ;
    }
}

static uint8_t get_byte_from_kb_buf()
{
    while (!kb_in.count) ; // wait next keystroke

    __asm__ volatile("cli");

    uint8_t scan_code = *(kb_in.tail++) ;
    if (kb_in.tail == kb_in.buf + KB_IN_BUF_LENGTH)
    {
        kb_in.tail = kb_in.buf ; // reset
    }
    kb_in.count-- ;
    __asm__ volatile("sti");

    return scan_code ;
}

void print_key(uint32_t k)
{
    // if FLAG_EXT is set, don't show this character
    if (!(k & FLAG_EXT))
    {
        char output[2] = {0} ;
        output[0] = k & 0xff ;
        show_msg(output) ;

        // adjust cursor
        __asm__ volatile("cli");
        outb(CLHR_INDEX, CRTCR_AR) ;
        outb(((position/2)>>8) & 0xff, CRTCR_DR) ;
        outb(CLLR_INDEX, CRTCR_AR) ;
        outb((position/2) & 0xff, CRTCR_DR) ;
        __asm__ volatile("sti");
    }
    else
    {
        uint32_t raw_code = k & MASK_RAW ;

        // try to implement move one line ;
        if (raw_code == UP)
        {
            // shift + up arrow
            if ((k & FLAG_SHIFT_L) || (k & FLAG_SHIFT_R))
            {
                __asm__ volatile("cli");
                outb(SAHR_INDEX, CRTCR_AR) ;
                outb(((80*15)>>8) & 0xff, CRTCR_DR) ;
                outb(SALR_INDEX, CRTCR_AR) ;
                outb((80*15) & 0xff, CRTCR_DR) ;
                __asm__ volatile("sti");
            }
        }
        else if (raw_code == DOWN)
        {
            if ((k & FLAG_SHIFT_L) || (k & FLAG_SHIFT_R))
            {
            }
        }
    }
}

void read_keyboard(CONSOLE *con)
{
    static bool code_e0 = false ;
    static bool l_shift = false ;
    static bool r_shift = false ;
    static bool l_alt = false ;
    static bool r_alt = false ;
    static bool l_ctrl = false ;
    static bool r_ctrl = false ;


    if (kb_in.count)
    {
        uint32_t key = 0 ;
        bool is_make = false;

        code_e0 = false ;

        uint8_t scan_code = get_byte_from_kb_buf() ;

        // handle special key code at first.
        if (scan_code == 0xe1)
        {
            // handle pause/break key code
            key = PAUSEBREAK ;
            uint8_t pause_key_code[] = {0xe1, 0x1d, 0x45, 0xe1, 0x9d, 0xc5} ;

            for (int x = 1 ; x < 6 ; ++x)
            {
                if (pause_key_code[x] != get_byte_from_kb_buf())
                {
                    key = 0 ;
                    break ;
                }
            }
        }
        else if (scan_code == 0xe0)
        {
            // handle 0xe0 key code
            scan_code = get_byte_from_kb_buf() ;

            // PrintScreen press
            if (scan_code == 0x2a)
                if (get_byte_from_kb_buf() == 0xe0)
                    if (get_byte_from_kb_buf() == 0x37)
                    {
                        key = PRINTSCREEN ;
                        is_make = true ;
                    }

            // PrintScreen release
            if (scan_code == 0xb7)
                if (get_byte_from_kb_buf() == 0xe0)
                    if (get_byte_from_kb_buf() == 0xaa)
                    {
                        key = PRINTSCREEN ;
                        is_make = false ;
                    }
            // if key does not have value, code_e0 flag set true
            if (!key) code_e0 = true ;
        }

        // handle normal key
        if (!key)
        {
            // handle make code and break code
            is_make = (scan_code & FLAG_BREAK) ? false : true ;

            uint32_t *row = &key_map[(scan_code & 0x7f) * MAP_COLS] ;

            uint8_t col = 0 ;

            bool caps = l_shift || r_shift ;
            if (caps_lock)
                if ((row[0] >= 'a' ) && row[0] <= 'z' )
                    caps = !caps ;

            if (caps) col = 1 ;
            if (code_e0) col = 2 ;
            /*
            if (l_shift || r_shift) col = 1 ;
            if (l_ctrl || r_ctrl) ;
            if (l_alt || r_alt) ;
            */

            // get key by row and col
            key = row[col] ;

            switch (key)
            {
            case SHIFT_L:
                l_shift = is_make ;
                break ;
            case SHIFT_R:
                r_shift = is_make ;
                break ;
            case CTRL_L:
                l_ctrl = is_make ;
                break ;
            case CTRL_R:
                r_ctrl = is_make ;
                break ;
            case ALT_L:
                l_alt = is_make ;
                break ;
            case ALT_R:
                r_alt = is_make ;
                break ;
            case NUM_LOCK:
                if (is_make)
                {
                    num_lock = !num_lock ;
                    set_kb_led() ;
                }
                break ;
            case CAPS_LOCK:
                if (is_make)
                {
                    caps_lock = !caps_lock ;
                    set_kb_led() ;
                }
                break ;
            case SCROLL_LOCK:
                if (is_make)
                {
                    scroll_lock = !scroll_lock ;
                    set_kb_led() ;
                }
                break ;
            default:
                //if(!is_make) key = 0 ;
                break ;
            }

            if (is_make)
            {
                bool pad = false ;
                // handle right-side calculator
                if (key >= PAD_SLASH && key <= PAD_9)
                {
                    pad = true ;
                    switch (key)
                    {
                    case PAD_SLASH:
                        key = '/' ;
                        break ;
                    case PAD_STAR:
                        key = '*' ;
                        break ;
                    case PAD_MINUS:
                        key = '-' ;
                        break ;
                    case PAD_PLUS:
                        key = '+' ;
                        break ;
                    case PAD_ENTER:
                        key = ENTER ;
                        break ;
                    default:
                        if (num_lock)
                        {
                            if (key >= PAD_0 && key <= PAD_9)
                                key = key - PAD_0 + '0' ;
                            else if (key == PAD_DOT)
                                key = '.' ;
                        }
                        else
                        {
                            switch(key)
                            {
                            case PAD_HOME:
                                key = HOME;
                                break;
                            case PAD_END:
                                key = END;
                                break;
                            case PAD_PAGEUP:
                                key = PAGEUP;
                                break;
                            case PAD_PAGEDOWN:
                                key = PAGEDOWN;
                                break;
                            case PAD_INS:
                                key = INSERT;
                                break;
                            case PAD_UP:
                                key = UP;
                                break;
                            case PAD_DOWN:
                                key = DOWN;
                                break;
                            case PAD_LEFT:
                                key = LEFT;
                                break;
                            case PAD_RIGHT:
                                key = RIGHT;
                                break;
                            case PAD_DOT:
                                key = DELETE;
                                break;
                            default:
                                break;
                            }
                        }
                        break ;
                    }
                }

                key |= (l_shift) ? FLAG_SHIFT_L : 0 ;
                key |= (r_shift) ? FLAG_SHIFT_R : 0 ;
                key |= (l_ctrl) ? FLAG_CTRL_L : 0 ;
                key |= (r_ctrl) ? FLAG_CTRL_R : 0 ;
                key |= (l_alt) ? FLAG_ALT_L : 0 ;
                key |= (r_alt) ? FLAG_ALT_R : 0 ;
                key |= (pad) ? FLAG_PAD : 0;

                store_key_into_console(con, key) ;
            }
        }
    }
}


/****************************************************************/
/****************************************************************/
/**************** SOFTWARE interrupt essential*******************/
/****************************************************************/
/****************************************************************/
void init_sw_interrupt_idt()
{
    init_idt_desc(INT_VECTOR_SYS_CALL, DA_386IGate, sys_call, PRI_USER) ;
}


int get_ticks()
{
    MESSAGE msg ;
    memset(&msg, 0, sizeof(MESSAGE)) ;

    msg.type = MSG_GET_TICKS ;
    send_recv(MSG_BOTH, SYSTEM_TASK, &msg) ; //system task

    // u.m3.m3i1 = Return value;
    return msg.u.m3.m3i1 ;
}

#if 0
extern void console_write(int con_id, char *buf, int len) ;
int sys_write(char *buf, int len, PROCESS *proc)
{
    console_write(proc->console_id, buf, len) ;
    return 0 ;
}
#endif

extern void console_write(int con_id, char *buf, int len) ;
int sys_printx(int un1, int un2, char *msg, PROCESS *proc)
{
    char *p ;
    if (int_reenter == 0) // RING 1~3
        p = vir_to_linear(GET_PROCESS_ID(proc), msg) ;
    else // RING 0
        p = msg ;

    /*
        if it's a CRITICAL or ASSERT + (RING0 || RING1)
        system hang.
    */
    if ((*p == MAGIC_CH_CRITICAL)
        ||(*p == MAGIC_CH_ASSERT && proc < & proc_table[RING0_TASK_CNT+RING1_TASK_CNT]))
    {
        __asm__ __volatile__("cli");
        char *vga = (char*)VGA_MEM_BASE ;
        char *m = p+1 ;

        while(vga < (char*)(VGA_MEM_BASE+VGA_MEM_SIZE))
        {
            if (*m == '\0') __asm__ __volatile__("hlt");

            *vga++ = *m++ ;
            *vga++ = 0x74 ; // red color
        }
    }


    // skip magic character
    if (*p && (*p == MAGIC_CH_ASSERT || *p == MAGIC_CH_CRITICAL))
        p++ ;

    if (*p)
        console_write(proc->console_id, p, strlen(p)) ;

    return 0 ;
}

/*
    function: send / receive
    src_dest: to / from (message)
    msg: message pointer
    p: caller process .
*/
int sys_send_recv(int func, int src_dest, MESSAGE *msg, PROCESS *proc)
{

    ASSERT(int_reenter == 0) ; // RING0 only
    // ASSERT((src_dest >= 0 && src_dest < TOTAL_TASK_CNT)) ;
    // ASSERT((src_dest >= 0 && src_dest < TOTAL_TASK_CNT) || src_dest == ANY || src_dest == INTERRUPT) ;

    int ret  = 0 ;

    MESSAGE *msg_la = vir_to_linear(proc->id, msg) ;
    msg_la->source = proc->id ; // assign caller index to msg_la

    ASSERT(msg_la->source != src_dest) ; // src and dest should be different

    switch(func)
    {
    case MSG_SEND:
        ret = msg_send(proc, src_dest, msg) ;
        if (ret) return ret ;
        break ;

    case MSG_RECEIVE:
        ret = msg_recv(proc, src_dest, msg) ;
        if (ret) return ret ;
        break ;

    default:
        CRITICAL("FUCK, NOT SUPPORT") ;
        break ;
    }

    return 0 ;
}

