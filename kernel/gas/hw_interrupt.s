#
#   Provide by Walon Li
#   2015/6/01
#
.include "kernel/gas/macro.inc"

.data

.code32

.extern     StackTop    # kernel StackTop
.extern     process_ready
.extern     show_msg
.extern     delay
.extern     hw_int_cnt

.extern     irq_table
.extern     clock_int_handler


# extern parameter
.extern     gdt_ptr
.extern     idt_ptr

.extern     tss
.extern     hw_int_cnt

//.extern     _restart_process
//.extern     _restart_re_enter

.text

.global     _restart_process
.global     _disable_irq
.global     _enable_irq



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


_restart_process:
    mov     process_ready, %esp
    lldt    PROC_LDT_SEL(%esp)
    lea     PROC_STACK_TOP(%esp), %eax
    movl    %eax,       (tss + TSS_ESP0)
_restart_re_enter:
    decl    hw_int_cnt
    pop     %gs
    pop     %fs
    pop     %es
    pop     %ds
    popal
    add     $4,         %esp
    iretl

_save:
    pushal
    push    %ds
    push    %es
    push    %fs
    push    %gs
    mov     %ss,    %dx
    mov     %dx,    %ds
    mov     %dx,    %es

    mov     %esp,   %eax

    incl    hw_int_cnt
    cmpl    $0,     hw_int_cnt
    jne     _save_1

    mov     $StackTop,   %esp
    pushl    $_restart_process
    jmp     (RET_ADDR_REG-PROC_STACK_BASE)(%eax)
_save_1:
    pushl    $_restart_re_enter
    jmp     (RET_ADDR_REG-PROC_STACK_BASE)(%eax)



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
.macro hw_int_master num
    call    _save
    # stop clock interrupt
    in      $INT_M_CTRL_MASK, %al
    or      $(1 << \num),     %al
    out     %al,    $INT_M_CTRL_MASK

    mov     $END_OF_INTERRUPT, %al
    out     %al,     $INT_M_CTRL

    sti
    pushl   $\num

    mov     clock_int_handler, %eax

    mov     (irq_table), %eax
    #call   (irq_table + (4 * \num) )
    lcall   $8, $irq_table
    jmp     .
    #call    (irq_table + (4 * \num) )


    jmp     .
    pop     %ecx
    cli

    # stop clock interrupt
    in      $INT_M_CTRL_MASK, %al
    and     $~(1 << \num),  %al
    out     %al,    $INT_M_CTRL_MASK

    ret
.endm


.align 16
_hw_int00:
    #master_8259     0
    hw_int_master   0

//restart:
//    mov     process_ready, %esp
//    lldt    PROC_LDT_SEL(%esp)
//    lea     PROC_STACK_TOP(%esp), %eax
//    movl    %eax,   (tss+TSS_ESP0)
//
//restart_reenter:
//    decl    hw_int_cnt
//
//    pop     %gs
//    pop     %fs
//    pop     %es
//    pop     %ds
//    popal
//
//    add     $4,     %esp
//
//    iretl

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


_disable_irq:
    mov     4(%esp),    %ecx
    pushf
    cli
    mov     $1,         %ah
    rol     %cl,        %ah
    cmp     $8,         %cl
    jae     _disable_S
_disable_M:
    in      $INT_M_CTRL_MASK, %al
    test    %ah,        %al
    jnz     _disable_done
    or      %ah,        %al
    out     %al,        $INT_M_CTRL_MASK
    popf
    mov     $1,         %eax
    ret
_disable_S:
    in      $INT_S_CTRL_MASK, %al
    test    %ah,        %al
    jnz     _disable_done
    or      %ah,        %al
    out     %al,        $INT_S_CTRL_MASK
    popf
    mov     $1,         %eax
    ret
_disable_done:
    popf
    xor     %eax,       %eax
    ret


_enable_irq:
    mov     4(%esp),    %eax
    pushf
    cli
    mov     $~1,        %ah
    rol     %cl,        %ah
    cmp     $8,         %cl
    jae     _enable_S
_enable_M:
    in      $INT_M_CTRL_MASK, %al
    and     %ah,        %al
    out     %al,        $INT_M_CTRL_MASK
    popf
    ret
_enable_S:
    in      $INT_S_CTRL_MASK, %al
    and     %ah,        %al
    out     %al,        $INT_S_CTRL_MASK
    popf
    ret












