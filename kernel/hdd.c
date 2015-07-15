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

// Hard Drive
#define SECTOR_SIZE         512
#define SECTOR_BITS         (SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT   9
static uint8_t hdd_status ;
static uint8_t hd_buf[SECTOR_SIZE * 2];

static void get_hdd_identify(uint8_t drive) ;
static bool get_status(int st_mask, int val, int timeout) ;
static void print_hdd_identify(uint16_t *info) ;

void hdd_task()
{
    MESSAGE msg ;

    hdd_init() ;
    while (true)
    {
        send_recv(MSG_RECEIVE, P_ANY, &msg) ; // try to receive hdd interrupt
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


static void get_hdd_identify(uint8_t drive)
{
    HDD_CMD cmd ;
    cmd.device = MAKE_DEV_REG(0, drive, 0) ;
    cmd.command = ATA_IDENTIFY ;

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

    // get data from port
    read_port(HDD_REG_DATA, hd_buf, SECTOR_SIZE) ;

    print_hdd_identify((uint16_t*)hd_buf) ;
}

typedef struct _ASCII_IDENTIFY
{
    int index ;
    int length ;
    char *description ;
} ASCII_IDENTIFY;

static void print_hdd_identify(uint16_t *info)
{
    ASCII_IDENTIFY identify[] = {{10, 20, "HD SN"},
                                 {27, 40, "HD Model"}} ;

    for (int i = 0 ; i < sizeof(identify)/sizeof(ASCII_IDENTIFY) ; ++i)
    {
        char s[64] = {0};
        char *p = (char*)&info[identify[i].index] ;
        for (int j = 0 ; j < identify[i].length/2 ; ++j)
        {
            s[i*2+1] = *p++ ;
            s[i*2] = *p++ ;
        }
        s[i*2] = 0 ;
        printf("%s %s\n", identify[i].description, s) ;
    }

    // bool support = false ;
    int capab = info[49] ;
    // support = (capab & 0x400) ? "Yes" : "No" ;
    printf("LBA support: %s\n", (capab & 0x200) ? "Yes" : "No" ) ;

    int cmd_set_sup = info[83] ;
    printf("LBA48 support: %s\n", (cmd_set_sup & 0x400) ? "Yes" : "No") ;

    int sector = (((int)info[64]<<16) + info[60]) ;
    printf("HDD size: %dMB\n", sector*512 / 1000000) ;
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






