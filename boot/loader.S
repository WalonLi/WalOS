#
#   Provide by Walon Li
#   2015/03/29
#

#include "pm.h"
.code16

.text
    jmp LABEL_BEGIN

#   GDT                         BASE            LIMIT               ATTR
LABEL_GDT:      Descriptor      0,              0,              0           
LABEL_CODE32_DESC:  Descriptor  0,              (Code32SegLen - 1), (DA_C + DA_32)
LABEL_DATA_DESC:    Descriptor  0,              (DataSegLen - 1),   DA_DRW
LABEL_STACK_DESC:   Descriptor  0,              (StackSegLen - 1),  (DA_DRW+DA_32)
LABEL_VIDEO_DESC:   Descriptor  0xb8000,        0xffff,             DA_DRW      

.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1
            .4byte      0            

#   GDT Selector
.set        Code32Selector, (LABEL_CODE32_DESC - LABEL_GDT)
.set        DataSelector,   (LABEL_DATA_DESC - LABEL_GDT)
.set        StackSelector,  (LABEL_STACK_DESC - LABEL_GDT)
.set        VideoSelector,  (LABEL_VIDEO_DESC - LABEL_GDT)


#####
LABEL_DATA_SEG:
PMMsg:      .asciz      "Switch to Protected mode success."
LDTMsg:     .asciz      "Jump to LDT segment success."
ColCount:   .byte       2

.set        PMMsgOffset,    (PMMsg - LABEL_DATA_SEG)
.set        LDTMsgOffset,   (LDTMsg - LABEL_DATA_SEG)
.set        ColCountOffset, (ColCount - LABEL_DATA_SEG)

.set        DataSegLen,     (. - LABEL_DATA_SEG)
#####



#####
LABEL_STACK_SEG:
.space      512,        0
.set        StackSegLen,    (. - LABEL_STACK_SEG)
#####



LABEL_BEGIN:

    # initial segments
    mov     %cs,        %ax
    mov     %ax,        %ds
    mov     %ax,        %ss
    mov     %ax,        %es
    mov     $0x100,     %sp


    # initial 32 code segment descriptor
    InitDescriptor LABEL_CODE32_SEG, LABEL_CODE32_DESC

    # initial data segment descriptor
    InitDescriptor LABEL_DATA_SEG, LABEL_DATA_DESC

    # initial stack segment descriptor
    InitDescriptor LABEL_STACK_SEG, LABEL_STACK_DESC


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
    ljmpl   $Code32Selector, $0
#.2byte      0xea66
#.4byte      0x00000000
#.2byte      SelectorCode32


LABEL_CODE32_SEG:
.code32
    mov     $DataSelector, %ax
    mov     %ax,        %ds     

    mov     $StackSelector, %ax
    mov     %ax,        %ss  

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    mov     $(StackSegLen-1), %esp

    call    ShowCStyleMsg

    jmp     .
#    mov $0xb800,    %ax                 # video segment
#    mov %ax,        %gs
#    mov $0xC,       %ah
#    mov $'L',       %al
#    mov %ax,        %gs:((80*0+39)*2)  # col:0 row:39
#    jmp .






ShowCStyleMsg:
    push    %eax
    push    %ebx
    push    %esi
    push    %edi

    # Get ColCount value
    xor     %eax,       %eax
    movb    %ds:(ColCountOffset), %al
    movb    $160,       %bl
    mul     %bl
    mov     %eax,       %edi

    movb    $0x7,       %ah
    xor     %esi,       %esi
    movl    $(PMMsgOffset), %esi

    cld 

Msg_1:
    # lodsb will push value to al
    # d flag is 0, then si++
    # d is 1, then di--
    lodsb
    cmp     $0,         %al
    je      Msg_2
    mov     %ax,        %gs:(%edi)
    add     $2,         %edi
    jmp     Msg_1

Msg_2:
    
    incb    %ds:(ColCountOffset)

    pop     %edi
    pop     %esi
    pop     %ebx
    pop     %eax

    ret

.set        Code32SegLen,   . - LABEL_CODE32_SEG




