#
#   Provide by Walon Li
#   2015/6/01
#
.include "kernel/gas/macro.inc"

.data
clock_int_msg:  .byte '^'

.code32

.extern     StackTop    # kernel StackTop
.extern     process_ready
.extern     show_msg
.extern     delay

.text

# 2 * i8259A interrupt controller
.global     _hw_int00
.global     _hw_int01
.global     _hw_int02
.global     _hw_int03
.global     _hw_int04
.global     _hw_int05
.global     _hw_int06
.global     _hw_int07
.global     _hw_int08
.global     _hw_int09
.global     _hw_int10
.global     _hw_int11
.global     _hw_int12
.global     _hw_int13
.global     _hw_int14
.global     _hw_int15

# hardware interrupt
.macro master_8259 num
    pushl   $(\num)
    call    hw_irq_handler
    add     $4,     %esp
    hlt
.endm

.macro slaver_8259 num
    pushl   $(\num)
    call    hw_irq_handler
    add     $4,     %esp
    hlt
.endm

.align 16
_hw_int00:
    #master_8259     0
    sub     $4,     %esp

    pushal
    push    %ds
    push    %es
    push    %fs
    push    %gs

    mov     %ss,    %dx
    mov     %dx,    %ds
    mov     %dx,    %es

    mov     $StackTop,   %esp

    incb    %gs:(0)
    mov     $END_OF_INTERRUPT, %al
    out     %al,     $INT_M_CTRL

    lea     PROC_STACK_TOP(%esp), %eax
    movl    %eax,   (tss+TSS_ESP0)

    sti
    push    $clock_int_msg
    call    show_msg
    add     $4,     %esp

    push    $10
    call    delay
    add     $4,     %esp
    cli


    mov     process_ready, %esp

    pop     %gs
    pop     %fs
    pop     %es
    pop     %ds
    popal

    add     $4,     %esp
    iretl

_hw_int01:
    master_8259     1

_hw_int02:
    master_8259     2

_hw_int03:
    master_8259     3

_hw_int04:
    master_8259     4

_hw_int05:
    master_8259     5

_hw_int06:
    master_8259     6

_hw_int07:
    master_8259     7

_hw_int08:
    slaver_8259     8

_hw_int09:
    slaver_8259     9

_hw_int10:
    slaver_8259     10

_hw_int11:
    slaver_8259     11

_hw_int12:
    slaver_8259     12

_hw_int13:
    slaver_8259     13

_hw_int14:
    slaver_8259     14

_hw_int15:
    slaver_8259     15
