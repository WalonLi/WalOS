/**
    Provide by Walon Li
    2015/7/1
**/


#ifndef __HDD_H__
#define __HDD_H__

#include "type.h"
#include "fs.h"

typedef struct _HDD_CMD         HDD_CMD ;
typedef struct _PARTITION_INFO  PARTITION_INFO ;
typedef struct _HDD_INFO        HDD_INFO ;
typedef struct _PARTITION_ENTRY PARTITION_ENTRY ;


// How to operate hard disk
// 1. Fill Command Block Register
// 2. Fill Control Block Register

// Command Block Registers
//      NAME                PORT                    INPUT/OUTPUT
#define HDD_REG_DATA        0x1f0                   // I/O

#define HDD_REG_FEATURES    0x1f1                   // O
#define HDD_REG_ERROR       0x1f1                   // I

#define HDD_REG_NSECTOR     0x1f2                   // I/O
#define HDD_REG_LBA_LOW     0x1f3                   // I/O
#define HDD_REG_LBA_MID     0x1f4                   // I/O
#define HDD_REG_LBA_HIGH    0x1f5                   // I/O
#define HDD_REG_DEV         0x1f6                   // I/O

#define HDD_REG_STATUS      0x1f7                   // I
    #define STATUS_BUSY         0x80
    #define STATUS_DRDY         0x40
    #define STATUS_DFSE         0x20
    #define STATUS_DSC          0x10
    #define STATUS_DRQ          0x08
    #define STATUS_CORR         0x04
    #define STATUS_IDX          0x02
    #define STATUS_ERR          0x01
#define HDD_REG_CMD         0x1f7                   // O


// Control Block Registers
#define HDD_REG_ALT_STATUS  0x3f6                   // I
#define HDD_REG_DEV_CTRL    0x3f6                   // O

#define HDD_REG_DRV_ADDR    0x3f7                   // I

//      LBA mode
// 7    1
// 6    L               (L == 0 ? CHS : LBA)
// 5    1
// 4    Choose Drive    (DRV == 0 ? master : slaver)
// 3    HS3             (if L == 0, 4bits select the head number)
// 2    HS2             (if L == 1, HS0~GS3 contain bit 24~27 of the LBA)
// 1    HS1
// 0    HS0
#define MAKE_CMD_DEVICE(lba, drv, lba_highest) (((lba) << 6) | ((drv) << 4) | (lba_highest & 0xF) | 0xA0)


#define	HDD_TIMEOUT         10000   // milli second
#define	PARTITION_TABLE_OFFSET  0x1BE
#define ATA_IDENTIFY        0xEC
#define ATA_READ            0x20
#define ATA_WRITE           0x30


struct _HDD_CMD
{
    uint8_t feature ;
    uint8_t n_sector ;
    uint8_t lba_low ;
    uint8_t lba_mid ;
    uint8_t lba_high ;
    uint8_t device ;
    uint8_t command ;
};

struct _PARTITION_INFO
{
    uint32_t base ;     // start sector
    uint32_t sec_cnt ;  // sector count
};

struct _HDD_INFO
{
    int open_cnt ;
    PARTITION_INFO primary[NR_PRIM_PER_DRIVE] ;
    PARTITION_INFO logical[NR_SUB_PER_DRIVE] ;
};

struct _PARTITION_ENTRY
{
    uint8_t indicator ;
    uint8_t start_head ;
    uint8_t start_sector ;
    uint8_t start_cylinder ;
    uint8_t system_id ;
    uint8_t end_head ;
    uint8_t end_sector ;
    uint8_t end_cylinder ;

    uint32_t starting_sector ;  // starting sector count from 0/LBA
    uint32_t sector_cnt ;           // total sector count in partition
};

void hdd_task() ;
void hdd_int_handler(int irq) ;
/*
void hdd_init() ;
void hdd_identify() ;
*/




// hard drive architecture
/*
offset  length  description
0       1       status(80h = leading,  00h = can't leading,  others = illegal)
1       1       start head number
2       1

...
*/

#endif

