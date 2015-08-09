/**
    Provide by Walon Li
    2015/8/4
**/

#ifndef __FS_H__
#define __FS_H__




typedef struct _DRIVE_MAP
{
    int drive_number ;
} DRIVE_MAP ;



/*
    primary master
-------------------------------------------------------------------------
|                                 hd0                                   |
-------------------------------------------------------------------------
|      hd1        |       hd2        |        hd3        |     hd4      |
-------------------------------------------------------------------------
| hd1a ~ hd1p     | hd2a ~ hd2p      | hd3a ~ hd3p       | hd4a ~ hd4p  |
-------------------------------------------------------------------------


    primary slaver
-------------------------------------------------------------------------
|                                 hd5                                   |
-------------------------------------------------------------------------
|      hd6        |       hd7        |        hd8        |     hd9      |
-------------------------------------------------------------------------
| hd6a ~ hd6p     | hd7a ~ hd7p      | hd8a ~ hd8p       | hd9a ~ hd9p  |
-------------------------------------------------------------------------

*/

#define MAX_DRIVES              2
#define NR_PART_PER_DRIVE       4       // 1~4
#define NR_SUB_PER_PART         16      // a~p

#define NR_SUB_PER_DRIVE        (NR_SUB_PER_PART * NR_PART_PER_DRIVE)
#define NR_PRIM_PER_DRIVE       (NR_PART_PER_DRIVE+1)

#define MAX_PRIM                (MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)
#define MAX_SUBPARTITIONS       (NR_SUB_PER_DRIVE * MAX_DRIVES)


#define NO_DEVICE               0
#define FLOPPY_DEVICE           1
#define CDROM_DEVICE            2
#define HDD_DEVICE              3
#define CONSOLE_DEVICE          4
#define SCSI_DEVICE             5

#define MAKE_DEV_SIGNATURE(a, b) ((a << 8) | b)
#define DEV_MAJOR(a)            ((a >> 8) & 0xff)
#define DEV_MINOR(a)            (a & 0xff)

#define MINOR_hd1a              0x10
#define MINOR_hd2a              (MINOR_hd1a+NR_SUB_PER_PART)

#define	P_PRIMARY	0
#define	P_EXTENDED	1

#define ORANGES_PART	0x99	// orange partition
#define NO_PART		0x00	    // no partition
#define EXT_PART	0x05	    // extend partition

#endif
