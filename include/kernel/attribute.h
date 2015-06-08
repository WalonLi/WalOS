/**
    Provide by Walon Li
    2015/6/8
**/


#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__


// Protected mode attribute
// Privilege
#define PRI_KRNL    0
#define PRI_TASK    1
#define PRI_USER    3


// 32 bit
#define    DA_32      0x4000
#define    DA_LIMIT_4K 0x8000

// DPL(Discriptor Privilege Level)
#define    DA_DPL0    0x0
#define    DA_DPL1    0x20
#define    DA_DPL2    0x40
#define    DA_DPL3    0x60

// Segment type
#define    DA_DR      0x90
#define    DA_DRW     0x92
#define    DA_DRWA    0x93
#define    DA_C       0x98
#define    DA_CR      0x9a
#define    DA_CCO     0x9c
#define    DA_CCOR    0x9e

#define    DA_LDT     0x82
#define    DA_TaskGate 0x85
#define    DA_386TSS  0x89
#define    DA_386CGate 0x8c
#define    DA_386IGate 0x8e
#define    DA_386TGate 0x8f


// selector attribute
#define     SA_RPL_MASK     0xfffc
#define     SA_RPL0         0
#define     SA_RPL1         1
#define     SA_RPL2         2
#define     SA_RPL3         3
#define     SA_TI_MASK      0xfffb
#define     SA_TIG          0
#define     SA_TIL          4

#define     RPL_KRNL    SA_RPL0
#define     RPL_TASK    SA_RPL1
#define     RPL_USER    SA_RPL3


#endif
