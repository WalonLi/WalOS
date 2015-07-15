/**
    Provide by Walon Li
    2015/7/1
**/


#ifndef __HDD_H__
#define __HDD_H__

#include "type.h"

typedef struct _HDD_CMD    HDD_CMD ;

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

#define HDD_REG_ALT_STATUS  0x3f6                   // I
#define HDD_REG_DEV_CTRL    0x3f6                   // O

#define HDD_REG_DRV_ADDR    0x3f7                   // I

#define MAKE_DEV_REG(lba, drv, lba_high) (((lba) << 6) | ((drv) << 4) |	(lba_high & 0xF) | 0xA0)

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


void hdd_task() ;
void hdd_init() ;
void hdd_identify() ;
void hdd_int_handler(int irq) ;
#endif

