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
#define DEV_TO_DRV(dev)     (dev <= MAX_PRIM ? dev / NR_PRIM_PER_DRIVE : (dev - MINOR_hd1a) / NR_SUB_PER_DRIVE)

static uint8_t hdd_status ;
static uint8_t hd_buf[SECTOR_SIZE * 2];
static HDD_INFO hdd_info[1] ;

static void get_hdd_identify(int drive) ;
static bool get_status(int st_mask, int val, int timeout) ;
static void print_hdd_identify(uint16_t *info) ;
static void hdd_open(int dev) ;

void hdd_task()
{
    MESSAGE msg ;
    printf("hdd task begin\n") ;

    hdd_init() ;
    while (true)
    {
        send_recv(MSG_RECEIVE, P_ANY, &msg) ; // try to receive hdd interrupt
        int src = msg.source ;

        switch (msg.type)
        {
        case MSG_TYPE_DEV_OPEN:
            hdd_open(msg.u.m3.m3i4) ; // device
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

    memset(&hdd_info, 0, sizeof(HDD_INFO)) ;
}

// hdd interrupt
void hdd_int_handler(int irq)
{
    hdd_status = inb(HDD_REG_STATUS) ;
    deliver_int_to_proc(HDD_TASK) ;
}

static void get_hdd_identify(int drive)
{
    HDD_CMD cmd ;
    cmd.device = MAKE_DEV_REG(0, drive, 0) ;
    cmd.command = ATA_IDENTIFY ;

    //
    // send and receive command
    //
    // check busy status at first.
    if (!get_status(STATUS_BUSY, 0 , HDD_TIMEOUT))
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

    uint16_t *info = (uint16_t*)hd_buf ;
    hdd_info[drive].primary[0].base = 0 ;
    hdd_info[drive].primary[0].sec_cnt = ((int)info[61] << 16) + info[60] ;
}

static void get_hdd_partition(int dev, int style)
{
    int drive = DEV_TO_DRV(dev) ;
    PARTITION_ENTRY part_table[NR_SUB_PER_DRIVE] ;

    if (style == P_PRIMARY)
    {
        // PRIMARY
        // get_partition_table() ;

    }
    else
    {
        // EXTENDED
    }
}

typedef struct _ASCII_IDENTIFY
{
    int index ;
    int length ;
    char *description ;
} ASCII_IDENTIFY;

static void print_hdd_identify(uint16_t *info)
{
    char s[64] = {0};
    ASCII_IDENTIFY identify[] = {{10, 20, "HD SN:"}, // 10~19
                                 {27, 40, "HD Model:"}, //27~46
                                 {23, 8,  "FW Ver:"}} ;

    for (int i = 0 ; i < sizeof(identify)/sizeof(ASCII_IDENTIFY) ; ++i)
    {
        int j = 0 ;
        char *p = (char*)&info[identify[i].index] ;
        for (; j < identify[i].length/2 ; ++j)
        {
            s[j*2+1] = *p++ ;
            s[j*2] = *p++ ;
        }
        s[j*2] = 0 ;
        printf("%s %s\n", identify[i].description, (strlen(s) ? s : "none") ) ;
    }

    // bool support = false ;
    int capab = info[49] ;
    // support = (capab & 0x400) ? "Yes" : "No" ;
    printf("LBA support: %s\n", (capab & 0x200) ? "Yes" : "No" ) ;

    int cmd_set_sup = info[83] ;
    printf("LBA48 support: %s\n", (cmd_set_sup & 0x400) ? "Yes" : "No") ;

    int sector = (((int)info[61]<<16) + info[60]) ;
    printf("HDD size: %dMB\n", sector*512 / 1000000) ;
}

static void print_hdd_info(HDD_INFO *info)
{

}

static void hdd_open(int dev)
{
    int drive = DEV_TO_DRV(dev) ;
    get_hdd_identify(drive) ;

    if (hdd_info[drive].open_cnt++ == 0)
    {
        // first time to open, print hdd information
        get_hdd_partition(drive * (NR_PART_PER_DRIVE+1), P_PRIMARY) ;
        print_hdd_info(&hdd_info[drive]) ;
    }
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






