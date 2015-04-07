#
#   Provide by Walon Li
#   2015/03/25
#


.code16
# common macro
.set        BaseOfStack,    0x7c00
.set        BaseOfLoader,   0x9000
.set        OffsetOfLoader, 0x0100      # Loading offset of LOADER.BIN
.set        RootDirSectors, 14          # Root directory sector count
.set        SecNoOfRootDir, 19          # 1st sector of root directory
.set        SecNoOfFAT1,    1           # 1st sector of FAT1
.set        DeltaSecNo,     17          # BPB_(RsvdSecCnt+NumFATs*FATSz) -2


.text
# Floppy header of FAT12 
    jmp LABEL_START                     # jmp boot 
    nop                                 # nop is required 
BS_OEMName:     .ascii      "WALON LI"  # OEM String, 8 bytes required 
BPB_BytsPerSec: .2byte      512         # Bytes per sector 
BPB_SecPerCluster: .byte    1           # Sector per cluster 
BPB_ResvdSecCnt:.2byte      1           # Reserved sector count 
BPB_NumFATs:    .byte       2           # Number of FATs 
BPB_RootEntCnt: .2byte      224         # Root entries count 
BPB_TotSec16:   .2byte      2880        # Total sector number 
BPB_Media:      .byte       0xf0        # Media descriptor 
BPB_FATSz16:    .2byte      9           # FAT size(sectors) 
BPB_SecPerTrk:  .2byte      18          # Sector per track 
BPB_NumHeads:   .2byte      2           # Number of magnetic heads 
BPB_HiddSec:    .4byte      0           # Number of hidden sectors 
BPB_TotSec32:   .4byte      0           # If TotSec16 equal 0, this works 
BS_DrvNum:      .byte       0           # Driver number of interrupt 13 
BS_Reserved1:   .byte       0           # Reserved 
BS_BootSig:     .byte       0x29        # Boot signal 
BS_VolID:       .4byte      0           # Volume ID 
BS_VolLab:      .ascii      "WalOS  0.01" # Volume label, 11 bytes required 
BS_FileSysType: .ascii      "FAT12 "    # File system type, 8 bytes required 


LABEL_START:
    mov     %cs,    %ax
    mov     %ax,    %ds
    mov     %ax,    %es
    mov     %ax,    %ss

    call    ClearScreen

    mov     $0,     %dh   
    call    ShowMsg

#    mov     $0,     %dh   
#    call    ShowMsg

#    mov     $1,     %dh   
#    call    ShowMsg

    # Reset floppy
    xor     %ah,    %ah
    xor     %dh,    %dh
    int     $0x13
#    jmp     .                           # infinite loop


    movw    $SecNoOfRootDir, (gSectorNo)   
LABEL_SEARCH_LOADER_IN_ROOT_DIR:
    cmpw    $0,         (gRootDirSizeForLoop)   # check root dir loop
    jz      LABEL_NO_LOADER
    decw    (gRootDirSizeForLoop)               # loop--

    # fill es:bx for int13 read
    # once read success, es:bx will have sector data
    mov     $BaseOfLoader, %ax
    mov     %ax,        %es                     
    mov     $OffsetOfLoader, %bx                # es:bx
    mov     (gSectorNo), %ax                    # %ax <- sector number
    mov     $1,         %cl                     # how many sector will be read
    call    ReadSector


    mov     $LoaderName, %si                    # ds:si = "LOADER  BIN"
    mov     $OffsetOfLoader, %di                # BaseOfLoader<<4+100
    cld
    mov     $0x10,      %dx


# Not found LOADER.BIN in root dir
LABEL_NO_LOADER:
    mov     $2,         %dh
    call    ShowMsg                             # show load fail message
    jmp     .




# data 
#
gRootDirSizeForLoop:    .2byte  RootDirSectors
gSectorNo:              .2byte  0               # Sector number to read
gOdd:                   .byte   0               # odd or even?

LoaderName:     .asciz      "LOADER  BIN"
PrintCount:     .byte       0

# Boot string table
Booting:        .ascii      "Booting......."
LoadSuccess:    .ascii      "Load success.."
LoadFail:       .ascii      "Load Fail....."
.set            MsgLen,     14




#
#   ClearScreen
#

ClearScreen:
    mov     $0x600,     %ax
    mov     $0x700,     %bx
    mov     $0,         %cx
    mov     $0x184f,    %dx         # {80,50}
    int     $0x10
    ret


#
#   ShowMsg
#
#   dh = Boot string table index

ShowMsg:
    mov     $MsgLen,    %ax
    mul     %dh
    add     $Booting,   %ax
    mov     %ax,        %bp

    # let es = ds
    mov     %ds,        %ax
    mov     %ax,        %es

    mov     $MsgLen,    %cx

    # call int 10h to show character
    mov     $0x1301,    %ax
    mov     $0x00c,     %bx
    mov     (PrintCount), %dh
    mov     $0,         %dl
    int     $0x10

    # PrintCount++
    incb    (PrintCount)
    #mov     (PrintCount), %ah
    #add     $1,         %ah
    #mov     %ah,        (PrintCount)
    ret  



#
#   ReadSector
#   	
#   Read %cl sector from %ax floppy sector to es:bx
#   x/(BPB_SecPerTrk) = y
#   x%(BPB_SecPerTrk) = z
#   z+1 = start sector number
#   y/BPB_NumHeads = cylinder number
#   y & 1 = magnetic header

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
Reading:
    mov     $2,     %ah
    mov     -2(%ebp), %al       # Read %al sectors 
    int     $0x13
    jc      Reading             # check CF, if == 1, read again
    add     $2,%esp
    pop     %ebp
    ret


.org 510            # fill null opcode until offset 510
.word 0xaa55        # end mark


