/**
    Provide by Walon Li
    2015/6/17
**/

#ifndef __VGA_H__
#define __VGA_H__

#define VGA_MEM_BASE 0xb8000
#define VGA_MEM_SIZE 0x8000 // 32KB

// Gerneral Registers
#define GR_MOR_READ     0x3cc
#define GR_MOR_WRITE    0x3c2
#define GR_ISR0_READ    0x3c2
#define GR_ISR1_READ    0x3da
#define GR_FCR_READ     0x3ca
#define GR_FCR_WRITE    0x3da
#defien GR_VSER         0x3c3

// Sequence Registers
#define SR_AR           0x3c4 // address
#define SR_DR           0x3c5 // data

// CRT Controller Regsiters
#define CRTCR_AR        0x3d4
#define CRTCR_DR        0x3d5

    // CRTCR index
    #define HTR_INDEX       0x0
    #define EHDR_INDEX      0x1
    #define SHBR_INDEX      0x2
    #define EHBR_INDEX      0x3
    #define SHRR_INDEX      0x4
    #define EHRR_INDEX      0x5
    #define VTR_INDEX       0x6
    #define OFR_INDEX       0x7 // Overflow register
    #define PRSR_INDEX      0x8
    #define MSLR_INDEX      0x9
    #define CSR_INDEX       0xa
    #define CER_INDEX       0xb
    #define SAHR_INDEX      0xc // Start Address high
    #define SALR_INDEX      0xd // Start Address low
    #define CLHR_INDEX      0xe // Cusor location high
    #define CLLR_INDEX      0xf // Cusro location low
    #define VRSR_INDEX      0x10
    #define VRER_INDEX      0x11
    #define VDER_INDEX      0x12
    #define OFFSETR_INDEX   0x13 // Offset
    #define ULR_INDEX       0x14
    #define SVBR_INDEX      0x15
    #define EVBR_INDEX      0x16
    #define MCR_INDEX       0x17
    #define LCR_INDEX       0x18


// Graphics Controller Registers
#define GCR_AR          0x3ce
#define GCR_DR          0x3cf

// Attriubte Controller Registers
#define ACR_AR          0x3c0
#define ACR_DR_READ     0x3c1
#define ACR_DR_WRITE    0x3c0

// Video DAC Palette Registers
#define VDAC_WA         0x3c8
#define VDAC_RA_WRITE   0x3c7
#define VDAC_DACS_READ  0x3c7
#define VDAC_D          0x3c9
#define VDAC_PM_READ    0x3c6

#endif
