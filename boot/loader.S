#
#   Provide by Walon Li
#   2015/03/29
#

#include "pm.h"
.code16

.text
    jmp LABEL_BEGIN

#   GDT                         BASE            LIMIT           ATTR
LABEL_GDT:      Descriptor      0,              0,              0           
LABEL_DESC_CODE32:  Descriptor  0,              SegCode32Len-1, DA_C + DA_32
LABEL_DESC_VIDEO:   Descriptor  0x0b8000,       0xffff,         DA_DRW      

.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1 
            .4byte      0            

#   GDT Selector
.set        SelectorCode32, LABEL_DESC_CODE32 - LABEL_GDT
.set        SelectorVideo,  LABEL_DESC_VIDEO - LABEL_GDT



LABEL_BEGIN:

    # initial segments
    mov     %cs,    %ax
    mov     %ax,    %ds
    mov     %ax,    %ss
    mov     %ax,    %es
    mov     $100,   %sp


    mov $0xb800,    %ax                 # video segment
    mov %ax,        %gs
    mov $0xC,       %ah
    mov $'L',       %al
    mov %ax,        %gs:((80*0+39)*2)  # col:0 row:39
    jmp .
    
.set        SegCode32Len,   . - LABEL_BEGIN