#
#   Provide by Walon Li
#   2015/5/16
#

.include "kernel/gas/macro.inc"

.code32

# extern function
.extern     init_pm_env
.extern     hw_irq_handler
.extern     inti_process_main

# extern parameter
.extern     gdt_ptr
.extern     idt_ptr

.extern     process_ready
.extern     tss


.bss
.global     StackTop

#.lcomm    StackSpace,     1024*2
StackSpace:     .space      1024*2,        0
StackTop:


.text

.global     _start

#.global     _restart_process

_start:
    mov     $StackTop,   %esp
    sgdtw   gdt_ptr

    call    init_pm_env

    # load gdt, idt, tss
    lgdtw   gdt_ptr
    lidtw   idt_ptr

    xor     %eax,       %eax
    mov     $SELECTOR_TSS, %ax
    ltr     %ax


    # jump to csinit to check GDT is work fine.
    # 0x8 is SELECTOR_KRNL_CS
    ljmp    $0x8, $csinit
csinit:

    #sti
    jmp     init_process_main
    # for test hardware excpetion
    #ljmp    $0x40, $0
    #ud2
    #hlt




















