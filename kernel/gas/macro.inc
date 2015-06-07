#
#   Provide by Walon Li
#   2015/6/01
#

# process stack offset
.set    PROC_STACK_BASE,    0
.set    GS_REG,             PROC_STACK_BASE+0
.set    FS_REG,             PROC_STACK_BASE+4
.set    ES_REG,             PROC_STACK_BASE+8
.set    DS_REG,             PROC_STACK_BASE+12
.set    EDI_REG,            PROC_STACK_BASE+16
.set    ESI_REG,            PROC_STACK_BASE+20
.set    EBP_REG,            PROC_STACK_BASE+24
.set    KRNL_ESP_REG,       PROC_STACK_BASE+28
.set    EBX_REG,            PROC_STACK_BASE+32
.set    EDX_REG,            PROC_STACK_BASE+36
.set    ECX_REG,            PROC_STACK_BASE+40
.set    EAX_REG,            PROC_STACK_BASE+44
.set    RET_ADDR_REG,       PROC_STACK_BASE+48
.set    EIP_REG,            PROC_STACK_BASE+52
.set    CS_REG,             PROC_STACK_BASE+56
.set    EFLAGS_REG,         PROC_STACK_BASE+60
.set    ESP_REG,            PROC_STACK_BASE+64
.set    SS_REG,             PROC_STACK_BASE+68
.set    PROC_STACK_TOP,     PROC_STACK_BASE+72

.set    PROC_LDT_SEL,       PROC_STACK_TOP
.set    PROC_LDT,           PROC_STACK_TOP+4

# TSS stack offset
.set    TSS_BASE,           0
.set    TSS_ESP0,           4
.set    TSS_SS0,            8
.set    TSS_ESP1,           12
.set    TSS_SS1,            16
.set    TSS_ESP2,           20
.set    TSS_SS2,            24


.set    SELECTOR_FLAT_C,    0x8
.set    SELECTOR_FLAT_RW,   0x10
.set    SELECTOR_VIDEO,     (0x18+3) #RPL3
.set    SELECTOR_TSS,       0x20

.set    SELECTOR_KRNL_CS,   SELECTOR_FLAT_C
.set    SELECTOR_KRNL_DS,   SELECTOR_FLAT_RW
.set    SELECTOR_KRNL_GS,   SELECTOR_VIDEO


.set    INT_M_CTRL,         0x20
.set    INT_M_CTRL_MASK,    0x21
.set    INT_S_CTRL,         0xa0
.set    INT_S_CTRL_MASK,    0xa1

.set    END_OF_INTERRUPT,   0x20
.set    EOI,                0x20

