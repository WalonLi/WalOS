#
#   Provide by Walon Li
#   2015/05/16
#

.set        BaseOfLoader,   0x9000
.set        OffsetOfLoader, 0x0100      # Loading offset of LOADER.BIN

.set        BaseOfLoaderPhyAddr, BaseOfLoader * 0x10


.set        BaseOfKernel,   0x8000
.set        OffsetOfKernel, 0

.set        BaseOfStack,    0x100


.set        PageDirBase,   0x100000
.set        PageTblBase,   0x101000



