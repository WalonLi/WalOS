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
LABEL_VIDEO_DESC:   Descriptor  0xb8000,        0xffff,             (DA_DRW+DA_DPL3)

LABEL_LDT_DESC:     Descriptor  0,              (LdtLen - 1),       DA_LDT

LABEL_R3_CODE_DESC: Descriptor  0,              (Ring3CodeSegLen - 1), (DA_C + DA_32 + DA_DPL3)
LABEL_R3_STACK_DESC: Descriptor 0,              (Ring3StackSegLen - 1), (DA_DRWA + DA_32 + DA_DPL3)

LABEL_TSS_DESC:     Descriptor  0,              (TssLen - 1),       DA_386TSS

LABEL_GATE_TEST_DESC: Descriptor 0,             (GateTestSegLen-1), (DA_C + DA_32)
#   Gate                        Selector            Offset  DCount  Attr
LABEL_GATE_TEST_CALL: Gate      GateTestSelector,   0,      0,      (DA_386CGate+DA_DPL3)


.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1
            .4byte      0            


#   LDT
LABEL_LDT:      
LABEL_LDT_A_DESC:   Descriptor  0,              (LdtASegLen - 1),  (DA_C + DA_32)

.set        LdtLen,     . - LABEL_LDT


#LABEL_GATE:



#   GDT Selector
.set        Code32Selector, (LABEL_CODE32_DESC - LABEL_GDT)
.set        DataSelector,   (LABEL_DATA_DESC - LABEL_GDT)
.set        StackSelector,  (LABEL_STACK_DESC - LABEL_GDT)
.set        VideoSelector,  (LABEL_VIDEO_DESC - LABEL_GDT)
.set        GateTestSelector, (LABEL_GATE_TEST_DESC - LABEL_GDT)
.set        LdtSelector,    (LABEL_LDT_DESC - LABEL_GDT)

.set        Ring3CodeSelector, (LABEL_R3_CODE_DESC - LABEL_GDT + SA_RPL3)
.set        Ring3StackSelector, (LABEL_R3_STACK_DESC - LABEL_GDT + SA_RPL3)

.set        TssSelector,    (LABEL_TSS_DESC - LABEL_GDT)
#   LDT Selector
.set        LdtASelector,   (LABEL_LDT_A_DESC - LABEL_LDT + SA_TIL)

#   Gate Selector
.set        GateTestCallSelector,   (LABEL_GATE_TEST_CALL - LABEL_GDT + SA_RPL3)


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

#####
LABEL_R3_STACK_SEG:
.space      512,        0
.set        Ring3StackSegLen,    (. - LABEL_R3_STACK_SEG)
#####

#####
LABEL_TSS:
    .4byte          0                       #Back
    .4byte          (StackSegLen - 1)       # ring0 stack
    .4byte          StackSelector           
    .4byte          0                       # ring1 stack
    .4byte          0
    .4byte          0                       # ring2 stack
    .4byte          0
    .4byte          0                       # CR3
    .4byte          0                       # EIP
    .4byte          0                       # EFLAGS
    .4byte          0                       # EAX
    .4byte          0                       # ECX
    .4byte          0                       # EDX
    .4byte          0                       # EBX
    .4byte          0                       # ESP
    .4byte          0                       # EBP
    .4byte          0                       # ESI
    .4byte          0                       # EDI
    .4byte          0                       # ES
    .4byte          0                       # CS
    .4byte          0                       # SS
    .4byte          0                       # DS
    .4byte          0                       # FS
    .4byte          0                       # GS
    .4byte          0                       # LDT
    .2byte          0                       # Test trap sign
    .2byte          (. - LABEL_TSS + 2)     # I/O base address
    .byte           0xff                    # I/O end sign

.set        TssLen,         (. - LABEL_TSS)
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

    # initial Ring3 code and stack
    InitDescriptor LABEL_R3_CODE_SEG, LABEL_R3_CODE_DESC
    InitDescriptor LABEL_R3_STACK_SEG, LABEL_R3_STACK_DESC

    # initial TSS
    InitDescriptor LABEL_TSS, LABEL_TSS_DESC


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


    # load TSS
    mov     $TssSelector, %ax
    ltr     %ax

    #Ring 3 test
    push    $Ring3StackSelector
    push    $(Ring3StackSegLen-1)
    push    $Ring3CodeSelector
    push    $0
    lret

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
    # d is 1, then si--
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


LABEL_R3_CODE_SEG:
    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    movl    $((80*13 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'3',       %al
    mov     %ax,        %gs:(%edi)

    lcall   $GateTestCallSelector, $0
    jmp     .

.set        Ring3CodeSegLen,     . - LABEL_R3_CODE_SEG
