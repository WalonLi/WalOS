#
#   Provide by Walon Li
#   2015/03/29
#

.include "pm.s"
.code16

.text
    jmp LABEL_BEGIN

#   GDT                         BASE            LIMIT               ATTR
LABEL_GDT:      Descriptor      0,              0,              0           
LABEL_CODE32_DESC:  Descriptor  0,              (Code32SegLen - 1), (DA_C + DA_32)
LABEL_DATA_DESC:    Descriptor  0,              (DataSegLen - 1),   DA_DRW
LABEL_STACK_DESC:   Descriptor  0,              (StackSegLen - 1),  (DA_DRW + DA_32)
LABEL_VIDEO_DESC:   Descriptor  0xb8000,        0xffff,             DA_DRW
LABEL_LDT_DESC:     Descriptor  0,              (LdtLen - 1),       DA_LDT

LABEL_GATE_TEST_DESC: Descriptor 0,             (GateTestSegLen-1), (DA_C + DA_32)
#   Gate                        Selector            Offset  DCount  Attr
LABEL_GATE_TEST_CALL: Gate      GateTestSelector,   0,      0,      (DA_386CGate+DA_DPL0)

.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1
            .4byte      0            


#   LDT
LABEL_LDT:      
LABEL_LDT_A_DESC:   Descriptor  0,              (LdtASegLen - 1),  (DA_C + DA_32)

.set        LdtLen,     . - LABEL_LDT


LABEL_GATE:



#   GDT Selector
.set        Code32Selector, (LABEL_CODE32_DESC - LABEL_GDT)
.set        DataSelector,   (LABEL_DATA_DESC - LABEL_GDT)
.set        StackSelector,  (LABEL_STACK_DESC - LABEL_GDT)
.set        VideoSelector,  (LABEL_VIDEO_DESC - LABEL_GDT)
.set        GateTestSelector, (LABEL_GATE_TEST_DESC - LABEL_GDT)
.set        LdtSelector,    (LABEL_LDT_DESC - LABEL_GDT)

#   LDT Selector
.set        LdtASelector,   (LABEL_LDT_A_DESC - LABEL_LDT + SA_TIL)

#   Gate Selector
.set        GateTestCallSelector,   (LABEL_GATE_TEST_CALL - LABEL_GDT)


#####
LABEL_DATA_SEG:
PMMsg:      .asciz      "Switch to Protected mode success."
LDTMsg:     .asciz      "Jump to LDT segment success."
ErrorMsg:   .asciz      "ERROR Message."
ColCount:   .byte       2

.set        PMMsgOffset,    (PMMsg - LABEL_DATA_SEG)
.set        LDTMsgOffset,   (LDTMsg - LABEL_DATA_SEG)
.set        ErrorMsgOffset, (ErrorMsg - LABEL_DATA_SEG)
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

    # initial gate test segment descriptor
    InitDescriptor LABEL_GATE_TEST_SEG, LABEL_GATE_TEST_DESC

    # initial LDT descriptor
    InitDescriptor LABEL_LDT, LABEL_LDT_DESC

    # initial LDT_A descriptor in LDT
    InitDescriptor LABEL_LDT_A_SEG, LABEL_LDT_A_DESC


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

    movb    $0,         %al
    call    ShowCStyleMsg

    #lcall   $GateTestSelector, $0
    lcall   $GateTestCallSelector, $0

    # jump to LDT_A
    mov     $LdtSelector, %ax
    lldt    %ax

    lcall   $LdtASelector, $0

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

    xor     %esi,       %esi
    # Get Msg by al
    cmpb    $0,         %al
    je      Msg0

    cmpb    $1,         %al
    je      Msg1

    jmp     MsgN

Msg0:    
    movl    $(PMMsgOffset), %esi
    jmp     MsgEnd

Msg1:    
    movl    $(LDTMsgOffset), %esi
    jmp     MsgEnd

MsgN:
    movl    $(LDTMsgOffset), %esi

MsgEnd:

    # Get ColCount value
    xor     %eax,       %eax
    movb    %ds:(ColCountOffset), %al
    movb    $160,       %bl
    mul     %bl
    mov     %eax,       %edi

    movb    $0x7,       %ah
    
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


LABEL_LDT_A_SEG:

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    movl    $((80*11 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'L',       %al
    mov     %ax,        %gs:(%edi)

    lret
.set        LdtASegLen,     . - LABEL_LDT_A_SEG


LABEL_GATE_TEST_SEG:

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    movl    $((80*12 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'G',       %al
    mov     %ax,        %gs:(%edi)

    lret
.set        GateTestSegLen,     . - LABEL_GATE_TEST_SEG
