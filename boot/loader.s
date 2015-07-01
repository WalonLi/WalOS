#
#   Provide by Walon Li
#   2015/5/16
#


.code16


.text
    jmp LABEL_BEGIN

.include "include/pm.s"
.include "include/common.s"
.include "include/fat12.s"
###########################
#   Descriptor Area       #
#                         #
###########################
#   GDT                         BASE            LIMIT               ATTR
LABEL_GDT:      Descriptor      0,              0,                  0
LABEL_FLAT_C_DESC:  Descriptor  0,              0xfffff,            (DA_CR + DA_32 + DA_LIMIT_4K)
LABEL_FLAT_RW_DESC: Descriptor  0,              0xfffff,            (DA_DRW + DA_32 + DA_LIMIT_4K)
LABEL_VIDEO_DESC:   Descriptor  0xb8000,        0xffff,             (DA_DRW+DA_DPL3)

# don't initial in code 16
#LABEL_PAGE_DIR_DESC: Descriptor PageDirBase,    4095,               DA_DRW
#LABEL_PAGE_TBL_DESC: Descriptor PageTblBase,    (4096*8-1),         DA_DRW      #DA_LIMIT_4K


.set        GdtLen,     . - LABEL_GDT
GdtPtr:     .2byte      GdtLen - 1
            .4byte      BaseOfLoaderPhyAddr + LABEL_GDT



###########################
#   Selector Area         #
#                         #
###########################
#   GDT Selector
.set        VideoSelector,  (LABEL_VIDEO_DESC - LABEL_GDT)

#.set        PageDirSelector, (LABEL_PAGE_DIR_DESC - LABEL_GDT)
#.set        PageTblSelector, (LABEL_PAGE_TBL_DESC - LABEL_GDT)
.set        FlatCSelector,  (LABEL_FLAT_C_DESC - LABEL_GDT)
.set        FlatRWSelector, (LABEL_FLAT_RW_DESC - LABEL_GDT)





#########################
#   Data segment        #
#                       #
#   Ring 0              #
#   R/W                 #
#########################
LABEL_DATA_SEG:

_gRootDirSizeForLoop:    .2byte  RootDirSectors
_gSectorNo:              .2byte  0               # Sector number to read
_gOdd:                   .byte   0               # odd or even?

_KernelName:     .asciz      "KERNEL  BIN"

# Boot string table
_Booting:        .ascii      "Boot to kernel.."
_LoadSuccess:    .ascii      "Load kernel ok.."
_LoadFail:       .ascii      "Load kernel Fail"
.set            MsgLen,     16

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


_PMMsg:      .asciz      "Switch to Protected mode success."
_LDTMsg:     .asciz      "Jump to LDT segment success."
_MemInfoMsg: .asciz      "BaseAddrL   BaseAddrH   LengthLow   LengthHigh  Type"
_MemDataMsg: .asciz      "00000000    00000000    00000000    00000000    00000000"
_MemSizeMsg: .asciz      "RAM size:   00000000"
_InterruptMsg: .asciz    "Open 8295A interrupt."
_PageMechanicMsg: .asciz "Enable Page Mechanic."
_PageTblCnt: .4byte      0
_ErrorMsg:   .asciz      "ERROR Message."
_MemByteCnt: .byte       0
_ColCount:   .byte       2


# PM mode data offset table
.set        MemSize,        (_MemSize + BaseOfLoaderPhyAddr)
.set        MCRNumber,      (_MCRNumber + BaseOfLoaderPhyAddr)
.set        ARDStruct,      (_ARDStruct + BaseOfLoaderPhyAddr)
    .set            BaseAddrLow,     (_BaseAddrLow + BaseOfLoaderPhyAddr)
    .set            BaseAddrHigh,    (_BaseAddrHigh + BaseOfLoaderPhyAddr)
    .set            LengthLow,       (_LengthLow + BaseOfLoaderPhyAddr)
    .set            LengthHigh,      (_LengthHigh + BaseOfLoaderPhyAddr)
    .set            Type,            (_Type + BaseOfLoaderPhyAddr)
.set        MemChkBuf,      (_MemChkBuf + BaseOfLoaderPhyAddr)


.set        PMMsg,    (_PMMsg + BaseOfLoaderPhyAddr)
.set        LDTMsg,   (_LDTMsg + BaseOfLoaderPhyAddr)
.set        MemInfoMsg, (_MemInfoMsg + BaseOfLoaderPhyAddr)
.set        MemDataMsg,   (_MemDataMsg + BaseOfLoaderPhyAddr)
.set        MemSizeMsg,   (_MemSizeMsg + BaseOfLoaderPhyAddr)
.set        InterruptMsg,   (_InterruptMsg + BaseOfLoaderPhyAddr)
.set        ErrorMsg, (_ErrorMsg + BaseOfLoaderPhyAddr)
.set        ColCount, (_ColCount + BaseOfLoaderPhyAddr)
.set        PageMechanicMsg, (_PageMechanicMsg + BaseOfLoaderPhyAddr)

.set        PageTblCnt, (_PageTblCnt + BaseOfLoaderPhyAddr)

.set        DataSegLen,     (. + BaseOfLoaderPhyAddr)


StackSeg:   .space      1024,        0
.set        TopOfStack, BaseOfLoaderPhyAddr + .

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
get_mem_info_loop:
    mov     $0xe820,    %eax
    mov     $20,        %ecx
    mov     $0x534d4150, %edx           # 'SMAP'
    int     $0x15
    jc      mem_chk_fail
    add     $20,        %di
    incl    _MCRNumber
    cmp     $0,         %ebx
    jne     get_mem_info_loop
    jmp     mem_chk_ok

mem_chk_fail:
    movl    $0,         _MCRNumber
mem_chk_ok:

    # Reset floppy
    xor     %ah,    %ah
    xor     %dh,    %dh
    int     $0x13

    movw    $SecNoOfRootDir, (_gSectorNo)
search_kernel_in_root_dir:
    cmpw    $0,         (_gRootDirSizeForLoop)   # check root dir loop
    jz      no_kernel
    decw    (_gRootDirSizeForLoop)               # loop--

    mov     $BaseOfKernel, %ax
    mov     %ax,        %es
    mov     $OffsetOfKernel, %bx                # es:bx
    mov     (_gSectorNo), %ax                    # %ax <- sector number
    mov     $1,         %cl                     # how many sector will be read
    call    ReadSector


    mov     $_KernelName, %si                    # ds:si = "LOADER  BIN"
    mov     $OffsetOfKernel, %di                # BaseOfLoader<<4+100
    cld

    mov     $0x10,      %dx                     # each sector have 0x10 x 0x20 byts
search_for_kernel_bin:
    cmp     $0,         %dx
    jz      next_sector_in_root_dir
    dec     %dx

    # compare file name
    mov     $11,        %cx
compare_file_name:
    cmp     $0,         %cx
    jz      file_name_found               # if cx == 0, file name is matche
    dec     %cx
    lodsb                                       # %ds:(%si) -> %al
    cmp     %es:(%di),  %al

    jz      next_character
    jmp     file_name_not_match


    # file name not matched, check next 0x20 bytes
file_name_not_match:
    and     $0xffe0,    %di
    add     $0x20,      %di
    mov     $_KernelName, %si
    jmp     search_for_kernel_bin

    # compare next character
next_character:
    inc     %di
    jmp     compare_file_name


next_sector_in_root_dir:
    addw    $1,         (_gSectorNo)
    jmp     search_kernel_in_root_dir


    # Not found LOADER.BIN in root dir
no_kernel:
    mov     $2,         %dh
    call    ShowMsgInRealMode                             # show load fail message
    jmp     .


    # file name found
file_name_found:
    mov     $RootDirSectors, %ax
    and     $0xffe0,    %di                     # reset Entry, now, es:di is entry of head
    add     $0x1a,      %di                     # offset 0x1a record first sector number
    mov     %es:(%di),  %cx
    push    %cx                                 # restore first sector to stack
    add     %ax,        %cx
    add     $DeltaSecNo, %cx                    # LOADER.BIN's start sector saved in %cl
    mov     $BaseOfKernel, %ax
    mov     %ax,        %es                     # %es <- BaseOfLoader
    mov     $OffsetOfKernel, %bx                # %bx <- OffsetOfLoader
    mov     %cx,        %ax                     # %ax <- Sector number

loading_file:
    #call    PrintDot

    mov     $1,         %cl
    call    ReadSector
    pop     %ax                                 # Got index of this sector in FAT
    call    GetFATEntry
    cmp     $0x0fff,    %ax                     # 0xfff means is the end sector
    jz      file_loaded
    push    %ax                                 # Save index of this sector in FAT
    mov     $RootDirSectors, %dx
    add     %dx,        %ax
    add     $DeltaSecNo, %ax
    add     (BPB_BytsPerSec), %bx
    jmp     loading_file

file_loaded:
    call    CloseMotor
    mov     $1,         %dh
    call    ShowMsgInRealMode                             # Show Load success

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
    ljmpl   $FlatCSelector, $(BaseOfLoaderPhyAddr+LABEL_PM_BEGIN)

    jmp     .
#.2byte      0xea66
#.4byte      0x00000000
#.2byte      SelectorCode32



##########################################################
#   ShowMsgInRealMode
#
#   dh = Boot string table index
##########################################################
ShowMsgInRealMode:
    mov     $MsgLen,    %ax
    mul     %dh
    add     $_Booting,   %ax
    mov     %ax,        %bp

    # let es = ds
    mov     %ds,        %ax
    mov     %ax,        %es

    mov     $MsgLen,    %cx

    # call int 10h to show character
    mov     $0x1301,    %ax

    cmp     $2,         %dh
    jz      msg_color_red
    jmp     msg_color_white

msg_color_red:
    mov     $0xc,       %bx
    jmp     msg_color_end
msg_color_white:
    mov     $0x7,       %bx
msg_color_end:

    mov     (_ColCount), %dh
    mov     $0,         %dl
    int     $0x10

    # PrintCount++
    incb    _ColCount
    ret



######################################################
#   ReadSector
#
#   Read %cl sector from %ax floppy sector to es:bx
#   x/(BPB_SecPerTrk) = y
#   x%(BPB_SecPerTrk) = z
#   z+1 = start sector number
#   y/BPB_NumHeads = cylinder number
#   y & 1 = magnetic header
######################################################
ReadSector:
    push    %ebp
    mov     %esp,   %ebp
    sub     $2,     %esp        # Reserve 2byte space
    mov     %cl,    -2(%ebp)    # Restore cl
    push    %bx
    mov     (BPB_SecPerTrk), %bl # bl is divider
    div     %bl                 # y in al, z in ah
    inc     %ah                 # z++, get start sector
    mov     %ah,    %cl
    mov     %al,    %dh
    shr     $1,     %al         # y/BPB_Numhead
    mov     %al,    %ch
    and     $1,     %dh
    pop     %bx

    # %ch = cylinder number
    # %cl = start sector number
    # %dh = magnetic header
    mov     (BS_DrvNum), %dl
reading:
    mov     $2,     %ah
    mov     -2(%ebp), %al       # Read %al sectors
    int     $0x13
    jc      reading             # check CF, if == 1, read again
    add     $2,%esp
    pop     %ebp
    ret

######################################################
#   GetFATEntry
#
######################################################
GetFATEntry:
    push    %es
    push    %bx
    push    %ax
    mov     $BaseOfKernel, %ax
    sub     $0x0100,    %ax
    mov     %ax,        %es                 # Left 4K bytes for FAT
    pop     %ax
    mov     $3,         %bx
    mul     %bx                             # %dx:%ax = %ax*3
    mov     $2,         %bx
    div     %bx                             # %dx:%ax/2
    movb    %dl,        (_gOdd)              # store remainder %dx in label bOdd.

    xor     %dx,        %dx                 # Now %ax is the offset of FATEntry in FAT
    mov     (BPB_BytsPerSec), %bx
    div     %bx                             # %dx:%ax/BPB_BytsPerSec
    push    %dx
    mov     $0,         %bx
    add     $SecNoOfFAT1, %ax               # %ax <- FATEntry sector
    mov     $2,         %cl                 # Read 2 sectors in 1 time, because FATEntry
    call    ReadSector                      # may be in 2 sectors.
    pop     %dx
    add     %dx,        %bx
    mov     %es:(%bx),  %ax                 # read FAT entry by word(2 bytes)
    cmpb    $0,         (_gOdd)             # remainder %dx(see above) == 0 ?
    jz      even_2                          # NOTE: %ah: high address byte, %al: low byte
    shr     $4,         %ax
even_2:
    and     $0x0fff,    %ax

    pop     %bx
    pop     %es
    ret


CloseMotor:
    push    %dx
    mov     $0x3f2,     %dx
    mov     $0,         %al
    out     %al,        %dx
    pop     %dx
    ret


##########################################################
#   LABEL_CODE32_SEG
#
#   Protected mode entry
#   Initial ds, ss, gs, esp, TSS and do anything.
##########################################################
LABEL_PM_BEGIN:
.code32

    mov     $VideoSelector, %ax
    mov     %ax,        %gs

    mov     $FlatRWSelector, %ax
    mov     %ax,        %ds
    mov     %ax,        %es
    mov     %ax,        %fs
    mov     %ax,        %ss
    mov     $TopOfStack, %esp


    movb    $0,         %al
    call    ShowMsgInProtectedMode

    # Sizing memory and load page table
    call    ShowMemInfo
    call    SetupPageMechanism

    call    InitKernel

    jmp     $FlatCSelector, $KernelEntryPhyAddr
    jmp     .
#    mov $0xb800,    %ax                 # video segment
#    mov %ax,        %gs
#    mov $0xC,       %ah
#    mov $'L',       %al
#    mov %ax,        %gs:((80*0+39)*2)  # col:0 row:39
#    jmp .



##########################################################
#   ShowMsgInProtectedMode
#
#   Show index:%al msg
##########################################################
ShowMsgInProtectedMode:
    push    %eax
    push    %ebx
    push    %esi
    push    %edi

    xor     %esi,       %esi
    # Get Msg by al
    cmpb    $0,         %al
    je      msg_0

    cmpb    $1,         %al
    je      msg_1

    cmpb    $2,         %al
    je      msg_2

    cmpb    $3,         %al
    je      msg_3

    cmpb    $4,         %al
    je      msg_4

    cmpb    $5,         %al
    je      msg_5

    cmpb    $6,         %al
    je      msg_6

    jmp     msg_error

msg_0:
    movl    $PMMsg, %esi
    jmp     msg_end

msg_1:
    movl    $LDTMsg, %esi
    jmp     msg_end

msg_2:
    movl    $MemInfoMsg, %esi
    jmp     msg_end

msg_3:
    movl    $MemDataMsg, %esi
    jmp     msg_end

msg_4:
    movl    $MemSizeMsg, %esi
    jmp     msg_end

msg_5:
    movl    $InterruptMsg, %esi
    jmp     msg_end

msg_6:
    movl    $PageMechanicMsg, %esi
    jmp     msg_end

msg_error:
    movl    $ErrorMsg, %esi

msg_end:

    # Get ColCount value
    xor     %eax,       %eax

    movb    %ds:(ColCount), %al
    movb    $160,       %bl
    mul     %bl
    mov     %eax,       %edi

    movb    $0x7,       %ah

    cld
show_msg:
    lodsb
    cmp     $0,         %al
    je      show_done
    mov     %ax,        %gs:(%edi)
    add     $2,         %edi
    jmp     show_msg
show_done:

    incb    %ds:(ColCount)

    pop     %edi
    pop     %esi
    pop     %ebx
    pop     %eax
    ret


################################################################
#   ShowMemInfo
#
#   MemChkBuf have memory ARD(Address Range Descriptor) struct
#   We parse this structure and show memory information on screen
################################################################
ShowMemInfo:
    push    %esi
    push    %edi
    push    %ecx

    movb    $2,         %al
    call    ShowMsgInProtectedMode

    movl    $MemChkBuf,     %esi
    movl    (MCRNumber),    %ecx
mem_info:
    push    %edi
    push    %ecx

    mov     $MemDataMsg,    %edi
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
    call    ShowMsgInProtectedMode
    call    CalculateMemSize
    loop    mem_info

    pop     %ecx
    pop     %edi
    pop     %esi

    ret


################################################################
#   ALtoAscii
#
#   in : AL
#   out: AX
#   Convert AL to ascii code and store into AX
################################################################
ALtoAscii:
    push    %bx
    push    %ax

    shr     $4,             %al
    and     $0xf,           %al

    cmpb    $0xa,           %al
    jb      add_ascii_30_1
    jmp     add_ascii_37_1
add_ascii_30_1:
    add     $0x30,          %al
    jmp     add_ascii_end_1
add_ascii_37_1:
    add     $0x37,          %al
add_ascii_end_1:
    mov     %al,            %bh

    pop     %ax
    and     $0xf,           %al

    cmpb    $0xa,           %al
    jb      add_ascii_30_2
    jmp     add_ascii_37_2
add_ascii_30_2:
    add     $0x30,          %al
    jmp     add_ascii_end_2
add_ascii_37_2:
    add     $0x37,          %al
add_ascii_end_2:
    mov     %al,            %bl
    mov     %bx,            %ax

    pop     %bx
    ret

################################################################
#   SetMemData
#
#   Set memory data to buffer by edx index
################################################################
SetMemData:
    # set memory data
    cmp     $5,             %edx
    je      set_mem_base

    cmp     $3,             %edx
    je      set_mem_length

    cmp     $1,             %edx
    je      set_mem_type
    jmp     set_mem_end

set_mem_base:
    movl    %eax,           (BaseAddrLow)
    jmp     set_mem_end
set_mem_length:
    movl    %eax,           (LengthLow)
    jmp     set_mem_end
set_mem_type:
    movl    %eax,           (Type)
set_mem_end:
    ret

################################################################
#   CalculateMemSize
#
#   Calulate memory size by base/length, stored into MemSize
################################################################
CalculateMemSize:
    cmpl    $1,             (Type)
    jne     cal_mem_end

    movl    (BaseAddrLow),  %eax
    addl    (LengthLow),    %eax
    cmpl    (MemSize),      %eax
    jb      cal_mem_end

    movl    %eax,           (MemSize)
cal_mem_end:
    ret


SetupPageMechanism:
    # For more easily, liner address match physical address
    # Each list have 4 byte
    # PD have 1024 PDE
    # PDE have 1 PT
    # PT have 1024 PTE
    # PTE have 1 PA(Physical Address)

    movb    $6,         %al
    call    ShowMsgInProtectedMode

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
    movl     %ecx,          (PageTblCnt)   # restore

    # initial Page Directory
    mov     $FlatRWSelector, %ax
    mov     %ax,            %es
    mov     $PageDirBase,   %edi
    xor     %eax,           %eax
    movl    $(PageTblBase  | PG_P | PG_USU | PG_RWW), %eax
init_dir:
    stosl
    add     $4096,          %eax
    loop    init_dir


    # initial Page Table
    mov     (PageTblCnt), %eax
    mov     $1024,          %ebx        # each
    mul     %ebx
    mov     %eax,           %ecx
    movl    $PageTblBase,   %edi
    xor     %eax,           %eax
    movl    $(PG_P | PG_USU | PG_RWW), %eax
init_tbl:
    stosl
    add     $4096,          %eax
    loop    init_tbl


    # load into CR3 and enable highest bit(PG) in CR0
    mov     $PageDirBase,   %eax
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

InitKernel:
	# copy kernel code to real offset by ELF header
	# 0x80000 -> 0x30400

    xor     %esi,           %esi
    movw    (BaseOfKernelPhyAddr + 0x2c), %cx	#ELF header->e_program header num
    movzx   %cx,            %ecx
    movl    (BaseOfKernelPhyAddr + 0x1c), %esi	#ELF header->e_program header offset
    add     $BaseOfKernelPhyAddr, %esi			#add kernel offset

init_kernel_begin:
    mov     (%esi),         %eax
    cmp     $0,             %eax
    jz      no_action
    pushl   0x10(%esi)							# size
    mov     4(%esi),        %eax
    add     $BaseOfKernelPhyAddr, %eax
    push    %eax								# src
    pushl   8(%esi)								# dest
    call    MemCpy
    add     $12,            %esp

no_action:
    add     $20,            %esi
    dec     %ecx
    jnz     init_kernel_begin
    ret
