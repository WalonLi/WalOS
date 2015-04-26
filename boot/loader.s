#
#   Provide by Walon Li
#   2015/04/23
#


#   Current Flow
#   1. Enter protected mode
#   2. Jump to Code32 segment(R0)
#   3. Show message.
#   4. Far Call Ring3 Code(R3) and show '3'
#   5. Far Call CG_Dest(R0) by call gate and show 'G'
#   5. Far Call LDT_A segemnt by LDT and show 'L'
#   6. Back to Ring3 Code(R3) and show 'O'

.include "pm.s"
.code16

.text
    jmp LABEL_BEGIN


###########################
#   Descriptor Area       #
#                         #
###########################
#   GDT                         BASE            LIMIT               ATTR
LABEL_GDT:      Descriptor      0,              0,              0           
LABEL_CODE32_DESC:  Descriptor  0,              (Code32SegLen - 1), (DA_C + DA_32)
LABEL_DATA_DESC:    Descriptor  0,              (DataSegLen - 1),   DA_DRW
LABEL_R0_STACK_DESC: Descriptor 0,              (Ring0StackLen - 1),  (DA_DRW + DA_32)
LABEL_VIDEO_DESC:   Descriptor  0xb8000,        0xffff,             (DA_DRW+DA_DPL3)

LABEL_LDT_DESC:     Descriptor  0,              (LdtLen - 1),       DA_LDT

LABEL_R3_CODE_DESC: Descriptor  0,              (Ring3CodeSegLen - 1), (DA_C + DA_32 + DA_DPL3)
LABEL_R3_STACK_DESC: Descriptor 0,              (Ring3StackLen - 1), (DA_DRWA + DA_32 + DA_DPL3)

LABEL_TSS_DESC:     Descriptor  0,              (TssLen - 1),       DA_386TSS

LABEL_CG_DEST_DESC: Descriptor  0,              (CGDestSegLen-1), (DA_C + DA_32)

#   Gate                        Selector            Offset  DCount  Attr
LABEL_CALL_GATE:    Gate        CGDestSelector,     0,      0,      (DA_386CGate+DA_DPL3)

.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1
            .4byte      0            


#   LDT
LABEL_LDT:      
LABEL_LDT_A_DESC:   Descriptor  0,              (LdtASegLen - 1),  (DA_C + DA_32)

.set        LdtLen,     . - LABEL_LDT



###########################
#   Selector Area         #
#                         #
###########################
#   GDT Selector
.set        Code32Selector, (LABEL_CODE32_DESC - LABEL_GDT)
.set        DataSelector,   (LABEL_DATA_DESC - LABEL_GDT)
.set        Ring0StackSelector,  (LABEL_R0_STACK_DESC - LABEL_GDT)
.set        VideoSelector,  (LABEL_VIDEO_DESC - LABEL_GDT)
.set        CGDestSelector, (LABEL_CG_DEST_DESC - LABEL_GDT)
.set        LdtSelector,    (LABEL_LDT_DESC - LABEL_GDT)

.set        Ring3CodeSelector, (LABEL_R3_CODE_DESC - LABEL_GDT + SA_RPL3)
.set        Ring3StackSelector, (LABEL_R3_STACK_DESC - LABEL_GDT + SA_RPL3)

.set        TssSelector,    (LABEL_TSS_DESC - LABEL_GDT)
#   LDT Selector
.set        LdtASelector,   (LABEL_LDT_A_DESC - LABEL_LDT + SA_TIL)

#   Gate Selector
.set        CallGateSelector,   (LABEL_CALL_GATE - LABEL_GDT + SA_RPL3)



#########################
#   Data segment        #
#                       #  
#   Ring 0              #
#   R/W                 #
#########################
LABEL_DATA_SEG:

# 16 bit mode data
_MemSize:   .4byte      0
_MCRNumber: .4byte      0                   # memory check result
_ARDStruct:                                 # Address Range Descriptor Structure
    _BaseAddrLow:     .4byte      0
    _BaseAddrHigh:    .4byte      0
    _LengthLow:       .4byte      0
    _LengthHigh:      .4byte      0
    _Type:            .4byte      0
_MemChkBuf: .space      256,        0



# 32 bit mode data
PMMsg:      .asciz      "Switch to Protected mode success."
LDTMsg:     .asciz      "Jump to LDT segment success."
MemInfoMsg: .asciz      "BaseAddrL   BaseAddrH   LengthLow   LengthHigh  Type"
MemMsg:     .asciz      "00000000    00000000    00000000    00000000    00000000"
ErrorMsg:   .asciz      "ERROR Message."
MemByteCnt: .byte       0
ColCount:   .byte       2



.set        MemSize,        (_MemSize - LABEL_DATA_SEG)
.set        MCRNumber,      (_MCRNumber - LABEL_DATA_SEG)
.set        ARDStruct,      (_ARDStruct - LABEL_DATA_SEG)
    .set            BaseAddrLow,     (_BaseAddrLow - LABEL_DATA_SEG)
    .set            BaseAddrHigh,    (_BaseAddrHigh - LABEL_DATA_SEG)
    .set            LengthLow,       (_LengthLow - LABEL_DATA_SEG)
    .set            LengthHigh,      (_LengthHigh - LABEL_DATA_SEG)
    .set            Type,            (_Type - LABEL_DATA_SEG)
.set        MemChkBuf,      (_MemChkBuf - LABEL_DATA_SEG)

.set        PMMsgOffset,    (PMMsg - LABEL_DATA_SEG)
.set        LDTMsgOffset,   (LDTMsg - LABEL_DATA_SEG)
.set        MemInfoMsgOffset, (MemInfoMsg - LABEL_DATA_SEG)
.set        MemMsgOffset,   (MemMsg - LABEL_DATA_SEG)
.set        ErrorMsgOffset, (ErrorMsg - LABEL_DATA_SEG)
.set        ColCountOffset, (ColCount - LABEL_DATA_SEG)

.set        DataSegLen,     (. - LABEL_DATA_SEG)



#########################
#   R0 Stack segment    #
#                       # 
#   Ring 0              #
#   R/W                 #
#   32 bits             #
#########################
LABEL_R0_STACK:
.space      512,        0
.set        Ring0StackLen,    (. - LABEL_R0_STACK)



#########################
#   R3 Stack segment    #
#                       #   
#   Ring 3              #
#   R/W                 #
#   32 bits             #
#########################
LABEL_R3_STACK:
.space      512,        0
.set        Ring3StackLen,    (. - LABEL_R3_STACK)



#############################
#   Task state segment      #
#                           # 
#   i386                    #   
#############################
LABEL_TSS:
    .4byte          0                       #Back
    .4byte          (Ring0StackLen - 1)       # ring0 stack
    .4byte          Ring0StackSelector           
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



##########################################################
#   LABEL_BEGIN             
#                            
#   Loader.bin real entry, do the below things.
#   1. Initial all descriptor.
#   2. Prepare GDTR.
#   3. Close interrupt for carefully.
#   4. Open A20 address line, it can access > 1mb mem now.
#   5. Enable PE bit(Protected Enviroment),
#   6. Mixed jump to code32 segment.
##########################################################
LABEL_BEGIN:

    # initial segments
    mov     %cs,        %ax
    mov     %ax,        %ds
    mov     %ax,        %ss
    mov     %ax,        %es
    mov     $0x100,     %sp

    
    # Get physical memory size and ARD struct
    mov     $0,         %ebx
    mov     $_MemChkBuf, %edi
GetMemInfoLoop:
    mov     $0xe820,    %eax
    mov     $20,        %ecx
    mov     $0x534d4150, %edx           # 'SMAP'
    int     $0x15
    jc      MemChkFail
    add     $20,        %di
    incl    _MCRNumber
    cmp     $0,         %ebx
    jne     GetMemInfoLoop
    jmp     MemChkOk

MemChkFail:
    movl    $0,         _MCRNumber
MemChkOk:

    
    # initial 32 code segment descriptor
    InitDescriptor LABEL_CODE32_SEG, LABEL_CODE32_DESC

    # initial data segment descriptor
    InitDescriptor LABEL_DATA_SEG, LABEL_DATA_DESC

    # initial stack segment descriptor
    InitDescriptor LABEL_R0_STACK, LABEL_R0_STACK_DESC

    # initial gate test segment descriptor
    InitDescriptor LABEL_CG_DEST_SEG, LABEL_CG_DEST_DESC

    # initial LDT descriptor
    InitDescriptor LABEL_LDT, LABEL_LDT_DESC

    # initial LDT_A descriptor in LDT
    InitDescriptor LABEL_LDT_A_SEG, LABEL_LDT_A_DESC

    # initial Ring3 code and stack
    InitDescriptor LABEL_R3_CODE_SEG, LABEL_R3_CODE_DESC
    InitDescriptor LABEL_R3_STACK, LABEL_R3_STACK_DESC

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




##########################################################
#   LABEL_CODE32_SEG        
#                           
#   Protected mode entry    
#   Initial ds, ss, gs, esp, TSS and do anything.
##########################################################
LABEL_CODE32_SEG:
.code32
    mov     $DataSelector, %ax
    mov     %ax,        %ds     

    mov     $Ring0StackSelector, %ax
    mov     %ax,        %ss  

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    mov     $(Ring0StackLen-1), %esp

    movb    $0,         %al
    call    ShowCStyleMsg

    movb    $2,         %al
    call    ShowCStyleMsg

    call    ShowMemInfo


    # load TSS
    mov     $TssSelector, %ax
    ltr     %ax

    #Ring 3 test
    push    $Ring3StackSelector
    push    $(Ring3StackLen-1)
    push    $Ring3CodeSelector
    push    $0
    lret

    #lcall   $CGDestSelector, $0
    #lcall   $CallGateSelector, $0

    # jump to LDT_A
    # mov     $LdtSelector, %ax
    # lldt    %ax

    # lcall   $LdtASelector, $0

    jmp     .
#    mov $0xb800,    %ax                 # video segment
#    mov %ax,        %gs
#    mov $0xC,       %ah
#    mov $'L',       %al
#    mov %ax,        %gs:((80*0+39)*2)  # col:0 row:39
#    jmp .

##########################################################
#   ShowCStyleMsg           
#                           
#   Show index:%al msg              
##########################################################
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

    cmpb    $2,         %al
    je      Msg2

    cmpb    $3,         %al
    je      Msg3

    jmp     MsgN

Msg0:    
    movl    $PMMsgOffset, %esi
    jmp     MsgEnd

Msg1:    
    movl    $LDTMsgOffset, %esi
    jmp     MsgEnd

Msg2:
    movl    $MemInfoMsgOffset, %esi
    jmp     MsgEnd

Msg3:
    movl    $MemMsgOffset, %esi
    jmp     MsgEnd

MsgN:
    movl    $LDTMsgOffset, %esi

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



ShowMemInfo:
    push    %esi
    push    %edi
    push    %ecx

    cld

    movl    $MemChkBuf,      %esi
    #movl    MCRNumber,      %ecx
ShowMemInfo.1:
    
    #mov     $ARDStruct,     %edi


    push    %edi
    push    %ecx

    mov     $MemMsgOffset,   %edi
    mov     $5,             %edx
x_1:
    #xor     %eax,           %eax
    lodsl
    mov     $4,             %ecx
x_2:
    push    %ax
    call    ALtoAscii
    
    movb    %ah,             %ds:(%edi)
    inc     %edi
    movb    %al,             %ds:(%edi)
    inc     %edi
    pop     %ax

    # next
    shr     $8,             %eax
    loop    x_2

    add     $4,             %edi
    dec     %edx
    cmp     $0,             %edx
    jne     x_1

    pop     %ecx
    pop     %edi

    mov     $3,             %al
    call    ShowCStyleMsg

    #loop    ShowMemInfo.1

    pop     %ecx
    pop     %edi
    pop     %esi
    ret


ALtoAscii:
    push    %bx
    push    %ax

    shr     $4,             %al
    and     $0xf,           %al

    cmpb    $0xa,           %al
    jb      AsciiAdd30
    jmp     AsciiAdd41
AsciiAdd30:
    add     $0x30,          %al
    jmp     AsciiEnd
AsciiAdd41:
    add     $0x37,          %al
    jmp     AsciiEnd
AsciiEnd:
    mov     %al,            %bh

    pop     %ax
    and     $0xf,           %al

    cmpb    $0xa,           %al
    jb      AsciiAdd30.2
    jmp     AsciiAdd41.2
AsciiAdd30.2:
    add     $0x30,          %al
    jmp     AsciiEnd.2
AsciiAdd41.2:
    add     $0x37,          %al
    jmp     AsciiEnd.2
AsciiEnd.2:
    mov     %al,            %bl
    mov     %bx,            %ax

    pop     %bx
    ret



.set        Code32SegLen,   . - LABEL_CODE32_SEG




    #and     $0xf            %al

    #cmp     $9,             %al


    #add     $0x30,          %al
    
    # Replace string
    #movb     %al,           %ds:(MemMsgOffset)


##########################################################
#   LABEL_LDT_A_SEG           
#                           
#   Test code with LDT.
#   If success, it will show 'L'          
##########################################################
LABEL_LDT_A_SEG:

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    movl    $((80*11 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'L',       %al
    mov     %ax,        %gs:(%edi)

    lret
.set        LdtASegLen,     . - LABEL_LDT_A_SEG




##########################################################
#   LABEL_CG_DEST_SEG           
#                           
#   Test code with Call Gate.
#   If success, it will show 'G'          
##########################################################
LABEL_CG_DEST_SEG:

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    movl    $((80*12 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'G',       %al
    mov     %ax,        %gs:(%edi)

    # jump to LDT_A
    mov     $LdtSelector, %ax
    lldt    %ax
    lcall   $LdtASelector, $0

    lret
.set        CGDestSegLen,     . - LABEL_CG_DEST_SEG




##########################################################
#   LABEL_R3_CODE_SEG           
#                           
#   Test code with enter RING3 or not.
#   If success, it will show '3' and 'O'      
##########################################################
LABEL_R3_CODE_SEG:
    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    movl    $((80*13 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'3',       %al
    mov     %ax,        %gs:(%edi)

    lcall   $CallGateSelector, $0

    movl    $((80*14 + 20) *2), %edi
    movb    $0xC,       %ah
    movb    $'O',       %al
    mov     %ax,        %gs:(%edi)
    jmp     .

.set        Ring3CodeSegLen,     . - LABEL_R3_CODE_SEG




    /**
    #push    %eax
    and     $0xf,           %al
    call    ConvertALtoAscii
    movb    %al,            %ds:(MemMsgOffset)

    #pop     %ax
    shr     $4,             %al
    call    ConvertALtoAscii
    movb    %al,            %ds:(MemMsgOffset+1)

    #pop     %ax
    shr     $4,             %al
    call    ConvertALtoAscii
    movb    %al,            %ds:(MemMsgOffset+1)

    #pop     %ax
    shr     $4,             %al
    call    ConvertALtoAscii
    movb    %al,            %ds:(MemMsgOffset+1)
    **/