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
LABEL_GDT:      Descriptor      0,              0,                  0           
LABEL_CODE32_DESC:  Descriptor  0,              (Code32SegLen - 1), (DA_CR + DA_32)
LABEL_DATA_DESC:    Descriptor  0,              (DataSegLen - 1),   DA_DRW
LABEL_R0_STACK_DESC: Descriptor 0,              (Ring0StackLen - 1),  (DA_DRW + DA_32)
LABEL_VIDEO_DESC:   Descriptor  0xb8000,        0xffff,             (DA_DRW+DA_DPL3)

LABEL_LDT_DESC:     Descriptor  0,              (LdtLen - 1),       DA_LDT

LABEL_R3_CODE_DESC: Descriptor  0,              (Ring3CodeSegLen - 1), (DA_C + DA_32 + DA_DPL3)
LABEL_R3_STACK_DESC: Descriptor 0,              (Ring3StackLen - 1), (DA_DRWA + DA_32 + DA_DPL3)

LABEL_TSS_DESC:     Descriptor  0,              (TssLen - 1),       DA_386TSS

# don't initial in code 16
#LABEL_PAGE_DIR_DESC: Descriptor PageDirBase,    4095,               DA_DRW
#LABEL_PAGE_TBL_DESC: Descriptor PageTblBase,    (4096*8-1),         DA_DRW      #DA_LIMIT_4K
LABEL_FLAT_C_DESC:  Descriptor  0,              0xfffff,            (DA_CR + DA_32 + DA_LIMIT_4K)
LABEL_FLAT_RW_DESC: Descriptor  0,              0xfffff,            (DA_DRW + DA_LIMIT_4K)



LABEL_CG_DEST_DESC: Descriptor  0,              (CGDestSegLen-1),   (DA_C + DA_32)

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
#.set        PageDirSelector, (LABEL_PAGE_DIR_DESC - LABEL_GDT)
#.set        PageTblSelector, (LABEL_PAGE_TBL_DESC - LABEL_GDT)
.set        FlatCSelector,  (LABEL_FLAT_C_DESC - LABEL_GDT)
.set        FlatRWSelector, (LABEL_FLAT_RW_DESC - LABEL_GDT)


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
MemDataMsg: .asciz      "00000000    00000000    00000000    00000000    00000000"
MemSizeMsg: .asciz      "RAM size:   00000000"   
PageTblCnt: .4byte      0
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
.set        MemDataMsgOffset,   (MemDataMsg - LABEL_DATA_SEG)
.set        MemSizeMsgOffset,   (MemSizeMsg - LABEL_DATA_SEG)
.set        ErrorMsgOffset, (ErrorMsg - LABEL_DATA_SEG)
.set        ColCountOffset, (ColCount - LABEL_DATA_SEG)

.set        PageTblCntOffset, (PageTblCnt - LABEL_DATA_SEG)

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

    # Sizing memory and load page table
    call    ShowMemInfo
    #call    SetupPageMechanism

    call    PagingDemo


    # load TSS
#    mov     $TssSelector, %ax
#    ltr     %ax

    #Ring 3 test
#    push    $Ring3StackSelector
#    push    $(Ring3StackLen-1)
#    push    $Ring3CodeSelector
#    push    $0
#    lret

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

    cmpb    $4,         %al
    je      Msg4

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
    movl    $MemDataMsgOffset, %esi
    jmp     MsgEnd

Msg4:
    movl    $MemSizeMsgOffset, %esi
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

    movb    $2,         %al
    call    ShowCStyleMsg

    movl    $MemChkBuf,     %esi
    movl    (MCRNumber),    %ecx
ShowMemInfo_1:
    push    %edi
    push    %ecx

    mov     $MemDataMsgOffset,  %edi
    mov     $5,             %edx
x_1:
    lodsl
    call    SetMemData
      
    push    %eax
    mov     $4,             %ecx
x_2:
    movl    (%esp),         %eax

    cmp     $1,             %ecx
    je      shf_end
    shr     $8,             %eax

    cmp     $2,             %ecx
    je      shf_end
    shr     $8,             %eax

    cmp     $3,             %ecx
    je      shf_end
    shr     $8,             %eax
shf_end:

    call    ALtoAscii
    movb    %ah,            %ds:(%edi)
    inc     %edi
    movb    %al,            %ds:(%edi)
    inc     %edi
    loop    x_2

    add     $4,             %esp
    add     $4,             %edi
    dec     %edx
    cmp     $0,             %edx
    jne     x_1

    pop     %ecx
    pop     %edi

    mov     $3,             %al
    call    ShowCStyleMsg

    call    CalMemSize

    loop    ShowMemInfo_1

    mov     $4,             %al
    call    ShowCStyleMsg

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
    jmp     AsciiAdd37
AsciiAdd30:
    add     $0x30,          %al
    jmp     AsciiEnd
AsciiAdd37:
    add     $0x37,          %al
AsciiEnd:
    mov     %al,            %bh

    pop     %ax
    and     $0xf,           %al

    cmpb    $0xa,           %al
    jb      AsciiAdd30.2
    jmp     AsciiAdd37.2
AsciiAdd30.2:
    add     $0x30,          %al
    jmp     AsciiEnd.2
AsciiAdd37.2:
    add     $0x37,          %al
AsciiEnd.2:
    mov     %al,            %bl
    mov     %bx,            %ax

    pop     %bx
    ret


SetMemData:
    # calculate ram size
    cmp     $5,             %edx
    je      set_base

    cmp     $3,             %edx
    je      set_length

    cmp     $1,             %edx
    je      set_type
    jmp     set_end

set_base:
    movl    %eax,           (BaseAddrLow)
    jmp     set_end
set_length:
    movl    %eax,           (LengthLow)
    jmp     set_end
set_type:
    movl    %eax,           (Type)
set_end:  
    ret


CalMemSize:
    cmpl    $1,             (Type)
    jne     cal_end

    movl    (BaseAddrLow),  %eax
    addl    (LengthLow),    %eax
    cmpl    (MemSize),      %eax
    jb      cal_end

    movl    %eax,           (MemSize)
cal_end:
    ret  


SetupPageMechanism:
    # For more easily, liner address match physical address
    # Each list have 4 byte
    # PD have 1024 PDE
    # PDE have 1 PT
    # PT have 1024 PTE
    # PTE have 1 PA(Physical Address)

    # calculate how many PDE and PTE to initial
    xor     %edx,           %edx
    mov     (MemSize),      %eax      
    mov     $0x400000,      %ebx        # each table can restore 4MB
    div     %ebx                        # eax = page table count = PDE count, %edx = remainder
    mov     %eax,           %ecx
    test    %edx,           %edx
    jz      no_remainder
    inc     %ecx
no_remainder:
    movl     %ecx,          (PageTblCntOffset)   # restore

    # initial Page Directory
    mov     $FlatRWSelector, %ax
    mov     %ax,            %es
    mov     $PageDirBase0,  %edi
    xor     %eax,           %eax
    movl    $(PageTblBase0 | PG_P | PG_USU | PG_RWW), %eax
init_dir:
    stosl
    add     $4096,          %eax
    loop    init_dir 



    # initial Page Table
    mov     (PageTblCntOffset), %eax
    mov     $1024,          %ebx        # each 
    mul     %ebx
    mov     %eax,           %ecx
    movl    $PageTblBase0,  %edi
    xor     %eax,           %eax
    movl    $(PG_P | PG_USU | PG_RWW), %eax
init_tbl:
    stosl
    add     $4096,          %eax
    loop    init_tbl



    # load into CR3 and enable highest bit(PG) in CR0
    mov     $PageDirBase0,  %eax
    mov     %eax,           %cr3

    mov     %cr0,           %eax
    or      $0x80000000,    %eax
    mov     %eax,           %cr0

    ret



# void *MemCpy(void* es:dest, ds:src, int size)
MemCpy:
    push    %ebp
    mov     %esp,           %ebp

    push    %esi
    push    %edi
    push    %ecx

    movl    8(%ebp),        %edi # dest
    movl    12(%ebp),       %esi # src
    movl    16(%ebp),       %ecx # counter

copy_ch:
    cmp     $0,             %ecx
    je      copy_end

    movb    %ds:(%esi),     %al
    inc     %esi
    mov     %al,            %es:(%edi)
    inc     %edi
    dec     %ecx
    jmp     copy_ch
copy_end:
    movl    8(%ebp),        %eax

    pop     %ecx
    pop     %edi
    pop     %esi
    mov     %ebp,           %esp
    pop     %ebp
    ret


#
PageSwitch:

    mov     $FlatRWSelector, %ax
    mov     %ax,            %es
    mov     $PageDirBase1,  %edi
    xor     %eax,           %eax
    movl    $(PageTblBase1 | PG_P | PG_USU | PG_RWW), %eax
    movl    (PageTblCntOffset), %ecx

    
switch_1:
    stosl
    add     $4096,          %eax
    loop    switch_1
    

    movl    (PageTblCntOffset), %eax
    shl     $10,            %eax    # eax * 1024
    mov     %eax,           %ecx
    mov     $PageTblBase1,  %edi
    xor     %eax,           %eax

    mov     $(PG_P | PG_USU | PG_RWW), %eax

    
switch_2:
    
    stosl
    add     $4096,          %eax

    loop    switch_2
    
    
    # assume memory > 8mb
    # backup oranges code
    #mov     $LinearAddrDemo, %eax
    #shr     $22,            %eax
    #mov     $4096,          %ebx
    #mul     %ebx
    #mov     %eax,           %ecx
    

    mov     $LinearAddrDemo, %eax
    shr     $12,            %eax
    shl     $2,             %eax    # eax * 4
    #and     $0x3ff,         %eax    # 1111111111b (10bit)
    #mov     $4,             %ebx 
    #mul     %ebx
    #add     %ecx,           %eax
    add     $PageTblBase1,  %eax
    movl    $(ProcBar | PG_P | PG_USU | PG_RWW), %es:(%eax)


    mov     $PageDirBase1,  %eax
    mov     %eax,           %cr3

    ret 



PagingDemo:

    # Move code seg to data seg
    mov     %cs,            %ax
    mov     %ax,            %ds
    mov     $FlatRWSelector, %ax
    mov     %ax,            %es
    


    push    $FooLen
    push    $FooOffset
    push    $ProcFoo
    call    MemCpy
    add     $12,            %esp

    push    $BarLen
    push    $BarOffset
    push    $ProcBar
    call    MemCpy
    add     $12,            %esp

    push    $PagingDemoProcLen
    push    $PagingDemoProcOffset
    push    $ProcPagingDemo
    call    MemCpy
    add     $12,            %esp

    # restore ds and es
    mov     $DataSelector,  %ax
    mov     %ax,            %ds
    mov     %ax,            %es

    call    SetupPageMechanism

    call    $FlatCSelector, $ProcPagingDemo

    call    PageSwitch

    call    $FlatCSelector, $ProcPagingDemo


    jmp     .
    ret 


PagingDemoProc:
.set        PagingDemoProcOffset, . - LABEL_CODE32_SEG
    mov     $LinearAddrDemo, %eax
    call    %eax
    lret
.set        PagingDemoProcLen,  . - PagingDemoProc


Foo:
.set        FooOffset,      . - LABEL_CODE32_SEG
    mov     $VideoSelector, %ax
    mov     %ax,        %gs
    mov     $0xc,           %ah
    mov     $'F',           %al
    mov     %ax,            %gs:((80*17)*2)
    ret  
.set        FooLen,         . - Foo


Bar:
.set        BarOffset,      . - LABEL_CODE32_SEG
    mov     $VideoSelector, %ax
    mov     %ax,        %gs
    mov     $0xc,           %ah
    mov     $'B',           %al
    mov     %ax,            %gs:((80*18)*2)
    ret  
.set        BarLen,         . - Bar



.set        Code32SegLen,   . - LABEL_CODE32_SEG




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

