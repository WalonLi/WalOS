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
###########################
#   FAT 12 table          #
#                         #
###########################
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
BS_FileSysType: .ascii      "FAT12   "  # File system type, 8 bytes required 


###########################
#   Global data           #
#                         #
###########################
gRootDirSizeForLoop:    .2byte  RootDirSectors
gSectorNo:              .2byte  0               # Sector number to read
gOdd:                   .byte   0               # odd or even?

LoaderName:     .asciz      "LOADER  BIN"
PrintCount:     .byte       0

# Boot string table
Booting:        .ascii      "Booting......"
LoadSuccess:    .ascii      "Load success."
LoadFail:       .ascii      "Load Fail...."
.set            MsgLen,     13



##########################################################################################
#   LABEL_BEGIN             
#                            
#   1. Clean screen and show "Booting...".
#   2. Reset floppy.
#   3. Search Loader.bin in root dir.
#      If found, get FAT entry and load n sector to memory, else show "Not Found",
#   4. Jump to Loader.bin
##########################################################################################
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


    mov     $0x10,      %dx                     # each sector have 0x10 x 0x20 byts
LABEL_SEARCH_FOR_LOADER_BIN:
    cmp     $0,         %dx
    jz      LABEL_NEXT_SECTOR_IN_ROOT_DIR
    dec     %dx

    # compare file name
    mov     $11,        %cx
LABEL_CMP_FILE_NAME:
    cmp     $0,         %cx
    jz      LABEL_FILE_NAME_FOUND               # if cx == 0, file name is matche
    dec     %cx
    lodsb                                       # %ds:(%si) -> %al
    cmp     %es:(%di),  %al
    
    jz      LABEL_NEXT_CHAR      
    jmp     LABEL_FILE_NAME_NOT_MATCH


    # file name not matched, check next 0x20 bytes
LABEL_FILE_NAME_NOT_MATCH:                      
    and     $0xffe0,    %di
    add     $0x20,      %di
    mov     $LoaderName, %si      
    jmp     LABEL_SEARCH_FOR_LOADER_BIN

    # compare next character
LABEL_NEXT_CHAR:
    inc     %di
    jmp     LABEL_CMP_FILE_NAME


LABEL_NEXT_SECTOR_IN_ROOT_DIR:
    addw    $1,         (gSectorNo)
    jmp     LABEL_SEARCH_LOADER_IN_ROOT_DIR


    # Not found LOADER.BIN in root dir
LABEL_NO_LOADER:
    mov     $2,         %dh
    call    ShowMsg                             # show load fail message
    jmp     .


    # file name found
LABEL_FILE_NAME_FOUND:
    mov     $RootDirSectors, %ax
    and     $0xffe0,    %di                     # reset Entry, now, es:di is entry of head
    add     $0x1a,      %di                     # offset 0x1a record first sector number
    mov     %es:(%di),  %cx
    push    %cx                                 # restore first sector to stack
    add     %ax,        %cx                     
    add     $DeltaSecNo, %cx                    # LOADER.BIN's start sector saved in %cl
    mov     $BaseOfLoader, %ax
    mov     %ax,        %es                     # %es <- BaseOfLoader
    mov     $OffsetOfLoader, %bx                # %bx <- OffsetOfLoader
    mov     %cx,        %ax                     # %ax <- Sector number

LABEL_LOADING_FILE:
    #call    PrintDot

    mov     $1,         %cl
    call    ReadSector
    pop     %ax                                 # Got index of this sector in FAT
    call    GetFATEntry
    cmp     $0x0fff,    %ax                     # 0xfff means is the end sector
    jz      LABEL_FILE_LOADED
    push    %ax                                 # Save index of this sector in FAT
    mov     $RootDirSectors, %dx
    add     %dx,        %ax
    add     $DeltaSecNo, %ax
    add     (BPB_BytsPerSec), %bx
    jmp     LABEL_LOADING_FILE

LABEL_FILE_LOADED:
    mov     $1,         %dh
    call    ShowMsg                             # Show Load success
    
    # Jump to LOADER.BIN start address in memory
    jmp     $BaseOfLoader, $OffsetOfLoader
 





##########################################################
#   ClearScreen          
#                                      
##########################################################
ClearScreen:
    mov     $0x600,     %ax
    mov     $0x700,     %bx
    mov     $0,         %cx
    mov     $0x184f,    %dx         # {80,50}
    int     $0x10
    ret



##########################################################
#   PrintDot       
#                                      
##########################################################
PrintDot:
    push    %ax
    push    %bx
    mov     $0xe,       %ah
    mov     $'.',       %al                     # print '.'
    mov     $0xf,       %bl
    int     $0x10
    pop     %bx
    pop     %ax
    ret



##########################################################
#   ShowMsg          
#        
#   dh = Boot string table index                    
##########################################################
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

    cmp	    $2,         %dh
    jz      MsgRedColor
    jmp     MsgWhiteColor

MsgRedColor:
    mov     $0xc,       %bx
    jmp     MsgFlag
MsgWhiteColor:
    mov     $0x7,       %bx

MsgFlag:
    mov     (PrintCount), %dh
    mov     $0,         %dl
    int     $0x10

    # PrintCount++
    incb    PrintCount
    #mov     (PrintCount), %ah
    #add     $1,         %ah
    #mov     %ah,        (PrintCount)
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
Reading:
    mov     $2,     %ah
    mov     -2(%ebp), %al       # Read %al sectors 
    int     $0x13
    jc      Reading             # check CF, if == 1, read again
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
    mov     $BaseOfLoader, %ax
    sub     $0x0100,    %ax
    mov     %ax,        %es                 # Left 4K bytes for FAT 
    pop     %ax
    mov     $3,         %bx
    mul     %bx                             # %dx:%ax = %ax*3 
    mov     $2,         %bx
    div     %bx                             # %dx:%ax/2 
    movb    %dl,        (gOdd)              # store remainder %dx in label bOdd. 
LABEL_EVEN:
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
    cmpb    $0,         (gOdd)              # remainder %dx(see above) == 0 ?
    jz      LABEL_EVEN_2                    # NOTE: %ah: high address byte, %al: low byte 
    shr     $4,         %ax
LABEL_EVEN_2:
    and     $0x0fff,    %ax
LABEL_GET_FAT_ENTRY_OK:
    pop     %bx
    pop     %es
    ret


################################
#   Boot signature(for BIOS)
#
################################
.org 510            # fill null opcode until offset 510
.word 0xaa55        # end mark


