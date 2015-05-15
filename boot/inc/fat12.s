/**
FAT12 COMMON SENSE 


Header
10     jmp short LABEL_START       ; Start to boot.
11     nop             ; 這個 nop 不可少
12 
13     ; 下面是 FAT12 磁碟的頭
14     BS_OEMName  DB 'Walon Li'   ; OEM String, 必須 8 個字元
15     BPB_BytsPerSec  DW 512      ; 每磁區字元數
16     BPB_SecPerClus  DB 1        ; 每叢集多少磁區
17     BPB_RsvdSecCnt  DW 1        ; Boot 記錄占用多少磁區
18     BPB_NumFATs DB 2        ; 共有多少 FAT 表
19     BPB_RootEntCnt  DW 224      ; 根目錄檔案數最大值
20     BPB_TotSec16    DW 2880     ; 邏輯磁區總數
21     BPB_Media   DB 0xF0     ; 媒體描述符
22     BPB_FATSz16 DW 9        ; 每FAT磁區數
23     BPB_SecPerTrk   DW 18       ; 每磁道磁區數
24     BPB_NumHeads    DW 2        ; 磁頭數(面數)
25     BPB_HiddSec DD 0        ; 隱藏磁區數
26     BPB_TotSec32    DD 0        ; wTotalSectorCount為0時這個值記錄磁區數
27     BS_DrvNum   DB 0        ; 中斷 13 的驅動器號
28     BS_Reserved1    DB 0        ; 未使用
29     BS_BootSig  DB 29h      ; 擴展引導標記 (29h)
30     BS_VolID    DD 0        ; 卷序列號
31     BS_VolLab   DB 'WalOS  0.01'; 卷標, 必須 11 個字元
32     BS_FileSysType  DB 'FAT12   '   ; 檔案系統類型, 必須 8個字元



2879---------------
|                 |
|   data section  |
|                 |
|   Root directory|
19-----------------
18                |
|                 |
|   FAT2          |
|                 |
10-----------------
9                 |
|                 |
|   FAT1          |
|                 |
1------------------
|   boot sector   |
0------------------
(sector number)


根目錄項目格式(32bytes)
Name            Offset      Length      Description
DIR_Name        0           0xB         name:8  subname:3, ex:Hello123.asm
DIR_Attr        0xB         1           file attribute
Reserved        0xC         10  
DIR_WrtTime     0x16        2           the latest write time
DIR_WrtDate     0x18        2           the latest write date
DIR_FstClus     0x1A        2           此項目對應的開始叢集號
DIR_FileSize    0x1C        4           File size



FAT1/2 Table
H                                               L
BYTE3            BYTE2             BYTE1
[B A 9 8 7 6 5 4 3 2 1 0] [B A 9 8 7 6 5 4 3 2 1 0]

Each FATEntry have 12bits, 0st and 1st is RESERVED 

ex:
0x200 F0 FF FF FF 8F 00 FF FF FF FF FF FF 09 A0 00 FF
0x210 0F 00 00 00 ...

If DIR_FstClus is 0x2, we can use table to get 0xFFF which means is end of cluster.
If DIR_FstClus is 0x3, 0x003 -> 0x008 -> 0x009 -> 0x00A -> 0xFFF(end), so this file occupy "4" cluster.


**/



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


.set        RootDirSectors, 14          # Root directory sector count
.set        SecNoOfRootDir, 19          # 1st sector of root directory
.set        SecNoOfFAT1,    1           # 1st sector of FAT1
.set        DeltaSecNo,     17          # BPB_(RsvdSecCnt+NumFATs*FATSz) -2
