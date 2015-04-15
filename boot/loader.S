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
LABEL_DESC_CODE32:  Descriptor  0,              (SegCode32Len - 1), (DA_C + DA_32)
LABEL_DESC_VIDEO:   Descriptor  0xb8000,        0xffff,         DA_DRW      

.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1
            .4byte      0            

#   GDT Selector
.set        SelectorCode32, (LABEL_DESC_CODE32 - LABEL_GDT)
.set        SelectorVideo,  (LABEL_DESC_VIDEO - LABEL_GDT)



LABEL_BEGIN:

    # initial segments
    mov     %cs,        %ax
    mov     %ax,        %ds
    mov     %ax,        %ss
    mov     %ax,        %es
    mov     $0x100,     %sp


    # initial 32 code segment descriptor
    xor     %eax,       %eax
    mov     %cs,        %ax
    shl     $4,         %eax
    addl    $LABEL_SEG_CODE32, %eax
    movw    %ax,        (LABEL_DESC_CODE32+2)
    shr     $16,        %eax
    movb    %al,        (LABEL_DESC_CODE32+4)
    movb    %ah,        (LABEL_DESC_CODE32+7)


    # for loading gdtr
    xor     %eax,       %eax
    mov     %ds,        %ax
    shl     $4,         %eax
    add     $LABEL_GDT, %eax
    movl    %eax,       (GdtPtr+2)

    # loading GDTR(Global Descriptor Table Register)
    lgdtw   GdtPtr

    # close all interrupt
    cli

    # open A20 address line
    inb     $0x92,      %al
    orb     $0x02,       %al
    outb    %al,        $0x92

    # For prepare switch protected mode, PE bit enable
    movl    %cr0,       %eax
    orl     $1,         %eax
    movl    %eax,       %cr0       


    # Mixed Jump
    ljmpl   $SelectorCode32, $0
#.2byte      0xea66
#.4byte      0x00000000
#.2byte      SelectorCode32


LABEL_SEG_CODE32:
.code32
    mov     $SelectorVideo, %ax
    mov     %ax,        %gs

    movl    $((80*11 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'L',       %al
    mov     %ax,        %gs:(%edi)

    jmp     .
#    mov $0xb800,    %ax                 # video segment
#    mov %ax,        %gs
#    mov $0xC,       %ah
#    mov $'L',       %al
#    mov %ax,        %gs:((80*0+39)*2)  # col:0 row:39
#    jmp .

.set        SegCode32Len,   . - LABEL_SEG_CODE32