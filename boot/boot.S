/**
    Provide by Walon Li
    2015/03/25
**/

.code16

.text
/* Floppy header of FAT12 */
    jmp LABEL_START                     /* jmp boot */
    nop                                 /* nop is required */
BS_OEMName:     .ascii      "WALON LI"  /* OEM String, 8 bytes required */
BPB_BytsPerSec: .2byte      512         /* Bytes per sector */
BPB_SecPerCluster: .byte    1           /* Sector per cluster */
BPB_ResvdSecCnt:.2byte      1           /* Reserved sector count */
BPB_NumFATs:    .byte       2           /* Number of FATs */
BPB_RootEntCnt: .2byte      224         /* Root entries count */
BPB_TotSec16:   .2byte      2880        /* Total sector number */
BPB_Media:      .byte       0xf0        /* Media descriptor */
BPB_FATSz16:    .2byte      9           /* FAT size(sectors) */
BPB_SecPerTrk:  .2byte      18          /* Sector per track */
BPB_NumHeads:   .2byte      2           /* Number of magnetic heads */
BPB_HiddSec:    .4byte      0           /* Number of hidden sectors */
BPB_TotSec32:   .4byte      0           /* If TotSec16 equal 0, this works */
BS_DrvNum:      .byte       0           /* Driver number of interrupt 13 */
BS_Reserved1:   .byte       0           /* Reserved */
BS_BootSig:     .byte       0x29        /* Boot signal */
BS_VolID:       .4byte      0           /* Volume ID */
BS_VolLab:      .ascii      "WalOS  0.01" /* Volume label, 11 bytes required */
BS_FileSysType: .ascii      "FAT12 "    /* File system type, 8 bytes required */

LABEL_START:
    mov %cs, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    call ShowMsg
    jmp .           # infinite loop
    
// ************************* //
// ************************* //
// ************************* //
ShowMsg:
    mov $Msg, %ax
    mov %ax, %bp
    mov $MsgLen, %cx

    # for int 10h
    mov $0x1301, %ax
    mov $0x00c, %bx
    mov $0, %dl
    int $0x10
    ret
Msg:    
    .ascii "Hello, Walon OS!"
    MsgLen = . - Msg    

// ************************* //
// ************************* //
// ************************* //

/**
    ReadSector
**/
ReadSector:
    push %ebp
    mov %esp,%ebp
    sub $2,%esp         /* Reserve space for saving %cl */
    mov %cl,-2(%ebp)    // Restore cl
    push %bx            /* Save bx */
    mov (BPB_SecPerTrk), %bl /* %bl: the divider */
    div %bl             /* 'y' in %al, 'z' in %ah */
    inc %ah             /* z++, got start sector */
    mov %ah,%cl         /* %cl <- start sector number */
    mov %al,%dh         /* %dh <- 'y' */
    shr $1,%al          /* 'y'/BPB_NumHeads */
    mov %al,%ch         /* %ch <- Cylinder number(y>>1) */
    and $1,%dh          /* %dh <- Magnetic header(y&1) */
    pop %bx             /* Restore %bx */
    /* Now, we got cylinder number in %ch, start sector number in %cl, magnetic
       header in %dh. */
    mov (BS_DrvNum), %dl
GoOnReading:
    mov $2,%ah
    mov -2(%ebp),%al    /* Read %al sectors */
    int $0x13
    jc GoOnReading      /* If CF set 1, mean read error, reread. */
    add $2,%esp
    pop %ebp
    ret


.org 510            #fill null opcode until offset 510
.word 0xaa55        #end mark


