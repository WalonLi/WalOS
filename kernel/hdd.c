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

static void handle_identify(int drive) ;
static void handle_partition(int dev, int style) ;
static bool get_status(int st_mask, int val, int timeout) ;
static void print_hdd_identify(uint16_t *info) ;
static void print_hdd_info(HDD_INFO *info) ;
static void hdd_init() ;
static void hdd_open(int dev) ;
static void hdd_cmd_out(HDD_CMD *cmd) ;
static void hdd_open(int dev) ;

/**
    hdd task main function
**/
void hdd_task()
{
    MESSAGE msg ;
    printf("hdd task begin\n") ;

    hdd_init() ;
    while (true)
    {
        // try to receive hdd interrupt
        send_recv(MSG_RECEIVE, P_ANY, &msg) ;
        int src = msg.source ;

        switch (msg.type)
        {
        case MSG_DEV_OPEN:
            hdd_open(msg.u.m3.m3i4) ; // device
            break ;
        default:
            CRITICAL("hdd_task") ;
            break ;
        }

        // send back to source
        send_recv(MSG_SEND, src, &msg) ;
    }
}

static void hdd_open(int dev)
{
    int drive = DEV_TO_DRV(dev) ;
    handle_identify(drive) ;

    if (hdd_info[drive].open_cnt++ == 0)
    {
        // first time to open, print hdd information
        handle_partition(drive * (NR_PART_PER_DRIVE+1), P_PRIMARY) ;
        print_hdd_info(&hdd_info[drive]) ;
    }
}

/**
    initial hdd
**/
static void hdd_init()
{
    // 0x475: Number of hard disk drives
    uint8_t *drives = (uint8_t*) 0x475 ;
    printf("drives:%d \n", *drives) ;

    ASSERT(*drives) ; // if no hdd, throw assert

    set_irq_handler(IRQ_AT, hdd_int_handler) ;
    enable_irq(IRQ_M_TO_S) ;
    enable_irq(IRQ_AT) ;

    memset(&hdd_info, 0, sizeof(HDD_INFO)) ;
}

/**
    hdd interrupt handler
**/
void hdd_int_handler(int irq)
{
    // get hdd status
    hdd_status = inb(HDD_REG_STATUS) ;

    // use IPC to deliver hdd interrupt to task
    deliver_int_to_proc(HDD_TASK) ;
}

/**
    get hdd identify data
**/
static void handle_identify(int drive)
{
    HDD_CMD cmd ;
    cmd.device = MAKE_CMD_DEVICE(0, drive, 0) ;
    cmd.command = ATA_IDENTIFY ;

    //
    // send and receive command
    //
    hdd_cmd_out(&cmd) ;

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

/**
    get partition table by (drive + sector)
**/
static void get_partition_table(int drive, int sector, PARTITION_ENTRY *tbl)
{
    HDD_CMD cmd ;
    cmd.feature = 0 ;
    cmd.n_sector = 1 ;
    cmd.lba_low = sector & 0xff ;
    cmd.lba_mid = (sector >> 8) & 0xff ;
    cmd.lba_high = (sector >> 16) & 0xff ;
    cmd.device = MAKE_CMD_DEVICE(1, drive, (sector >> 24) & 0xf) ;
    cmd.command = ATA_READ ;

    hdd_cmd_out(&cmd) ;

    // wait hdd interrupt...
    MESSAGE msg ;
    send_recv(MSG_RECEIVE, MSG_SOURCE_INTERRUPT, &msg) ;

    // get data from port
    read_port(HDD_REG_DATA, hd_buf, SECTOR_SIZE) ;

    memcpy(tbl, hd_buf+PARTITION_TABLE_OFFSET, sizeof(PARTITION_ENTRY) * NR_PART_PER_DRIVE) ;
}

/**
    get hdd partition
**/
static void handle_partition(int dev, int style)
{
    int drive = DEV_TO_DRV(dev) ;
    PARTITION_ENTRY table[NR_SUB_PER_DRIVE] ;

    if (style == P_PRIMARY)
    {
        // PRIMARY
        printf("drive:%d\n", drive) ;
        get_partition_table(drive, drive, table) ;

        for (int i = 0 ; i < NR_PART_PER_DRIVE ; i++) // 0~3
        {
            if (table[i].system_id == 0x0) // no partition
                continue ;

            // fill primary base and sector count
            int dev_nr = i + 1 ;
            hdd_info[drive].primary[dev_nr].base = table[i].starting_sector ;
            // printf("%x %x\n", table[i].start_sector, table[i].starting_sector) ;
            hdd_info[drive].primary[dev_nr].sec_cnt = table[i].sector_cnt ;
            if (table[i].system_id == EXT_PART)
            {
                handle_partition(dev + dev_nr, P_EXTENDED) ;
            }
        }
    }
    else
    {
        // EXTENDED
        int j = dev % NR_PRIM_PER_DRIVE ; // 1~4

        int extend_base_sector = hdd_info[drive].primary[j].base ;
        int next_sub_sector = extend_base_sector;

        int sub_signature = (j-1) * NR_SUB_PER_PART ; // 0/16/32/48

        for (int i = 0 ; i < NR_SUB_PER_PART ; i++)
        {
            int dev_nr = sub_signature + i ;
            get_partition_table(drive, next_sub_sector, table) ;

            hdd_info[drive].logical[dev_nr].base = next_sub_sector + table[0].starting_sector ;
            hdd_info[drive].logical[dev_nr].sec_cnt = table[0].sector_cnt ;
            next_sub_sector = extend_base_sector + table[1].starting_sector ;

            if (table[1].system_id == NO_PART)
                break ;
        }
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

    for (int i = 0 ; i < NR_PART_PER_DRIVE + 1 ; i++)
    {
        printf("%s", (i == 0 ? " DRIVE_" : "     MAIN_")) ;
        printf("PART_%d-> base:0x%x sector_cnt:0x%x\n", i, info->primary[i].base, info->primary[i].sec_cnt) ;
    }

    for (int i = 0 ; i < NR_SUB_PER_DRIVE ; i++)
    {
        if (!info->logical[i].sec_cnt) continue ;

        printf("         ") ;
        printf("LOGIC_PART_%d-> base:0x%x, sector_cnt:0x%x\n", i, info->logical[i].base, info->logical[i].sec_cnt) ;
    }
}

static void hdd_cmd_out(HDD_CMD *cmd)
{
    // check busy status at first.
    if (!get_status(STATUS_BUSY, 0 , HDD_TIMEOUT))
        CRITICAL("fuck, hard drive always busy") ;

    // enable interrupt(nIEN)
    outb(0, HDD_REG_DEV_CTRL) ;

    // Out Command Block Register
    outb(cmd->feature, HDD_REG_FEATURES) ;
    outb(cmd->n_sector, HDD_REG_NSECTOR) ;
    outb(cmd->lba_low, HDD_REG_LBA_LOW) ;
    outb(cmd->lba_mid, HDD_REG_LBA_MID) ;
    outb(cmd->lba_high, HDD_REG_LBA_HIGH) ;
    outb(cmd->device, HDD_REG_DEV) ;
    // out command
    outb(cmd->command, HDD_REG_CMD) ;
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






