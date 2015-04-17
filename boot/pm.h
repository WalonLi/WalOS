/*
*   Provide by Walon Li
*   2015/03/25
*/


/*
*   Desciptor Attribute
*   D   :   Data Segment
*   C   :   Code Segment
*   S   :   System Segment
*   R   :   Read
*   RW  :   Read/Write
*   A   :   Access
*/

// 32 bit
.set    DA_32,      0x4000


// DPL(Discriptor Privilege Level)
.set    DA_DPL0,    0x0
.set    DA_DPL1,    0x20
.set    DA_DPL2,    0x40
.set    DA_DPL3,    0x60

// Segment type
.set    DA_DR,      0x90
.set    DA_DRW,     0x92
.set    DA_DRWA,    0x93
.set    DA_C,       0x98
.set    DA_CR,      0x9a
.set    DA_CCO,     0x9c
.set    DA_CCOR,    0x9e


.set    DA_LDT,     0x82
.set    DA_TaskGate, 0x85
.set    DA_386TSS,  0x89
.set    DA_386CGate, 0x8c
.set    DA_386IGate, 0x8e
.set    DA_386TGate, 0x8f


/*  InitDescriptor
*   
*   
*   
*/
.macro InitDescriptor   Seg, Desc
    xor     %eax,       %eax
    mov     %cs,        %ax
    shl     $4,         %eax
    addl    $(\Seg),    %eax
    movw    %ax,        (\Desc+2)
    shr     $16,        %eax
    movb    %al,        (\Desc+4)
    movb    %ah,        (\Desc+7)
.endm

/*  Descriptor
*   Base    : 4byte
*   Limit   : 4byte
*   Attr    : 2byte
*/
.macro Descriptor Base, Limit, Attr
    .2byte  \Limit & 0xffff
    .2byte  \Base & 0xffff
    .byte   (\Base >> 16) & 0xff
    .2byte  ((\Limit >> 8) & 0xf00) | (\Attr & 0xf0ff)
    .byte   (\Base >> 24) & 0xff
.endm