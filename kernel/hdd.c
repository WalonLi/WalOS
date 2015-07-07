/**
    Provide by Walon Li
    2015/7/1
**/

#include "kernel/process.h"
#include "kernel/interrupt.h"
#include "kernel/hdd.h"
#include "kernel/io.h"
#include "lib/string.h"
#include "lib/debug.h"

static uint8_t hdd_status ;

void get_identify(uint8_t drive) ;

void hdd_task()
{
    MESSAGE msg ;

    hdd_init() ;
    while (true)
    {
        send_recv(MSG_RECEIVE, P_ANY, &msg) ;
        int src = msg.source ;

        switch (msg.type)
        {
        case MSG_TYPE_DEV_OPEN:
            get_hdd_identify(0) ;
            break ;
        default:
            CRITICAL("hdd_task") ;
            break ;
        }
        send_recv(MSG_SEND, src, &msg) ;
    }
}

void hdd_init()
{
    uint8_t *drives = (uint8_t*) 0x475 ;
    printf("drives:%d \n", *drives) ;

    ASSERT(*drives) ; // if no hdd, throw assert

    set_irq_handler(IRQ_AT, hdd_int_handler) ;
    enable_irq(IRQ_M_TO_S) ;
    enable_irq(IRQ_AT) ;
}

// hdd interrupt
void hdd_int_handler(int irq)
{
    hdd_status = inb(HDD_REG_STATUS) ;
    deliver_int_to_proc(HDD_TASK) ;
}


static void get_identify(uint8_t drive)
{
    HDD_CMD cmd ;
    cmd.dev = MAKE_DEV_REG(0, drive, 0) ;
    cmd.cmd = ATA_IDENTIFY ;

    //
    // send and receive command
    //
    // check busy status at first.
    if (get_status(STATUS_BUSY, 0 , HDD_TIMEOUT))
        CRITICAL("fuck, hard drive always busy") ;

    // enable interrupt(nIEN)
    outb(0, HDD_REG_DEV_CTRL) ;

    // Out Command Block Register
    outb(cmd.feature, HDD_REG_FEATURES) ;
    outb(cmd.n_sector, HDD_REG_NSECTOR) ;
    outb(cmd.lba_low, HDD_REG_LBA_LOW) ;
    outb(cmd.lba_mid, HDD_REG_LBA_MID) ;
    outb(cmd.lba_high, HDD_REG_LBA_HIGH) ;
    outb(cmd.device, HDD_REG_DEV) ;
    // out command
    outb(cmd.command, HDD_REG_CMD) ;

    // wait hdd interrupt...
    MESSAGE msg ;
    send_recv(MSG_RECEIVE, MSG_SOURCE_INTERRUPT, &msg) ;
}

#define HZ 100
extern int get_ticks() ;
static bool get_status(int st_mask, int val, int timeout)
{
    for (int t = get_ticks() ; ((get_ticks()-t) * 1000/HZ) < timeout ; )
    {
        if ((inb(HDD_REG_STATUS) & st_mask) == val)
            return true ;
    }
    return false ;
}






