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
.extern     int_reenter
.extern     sys_call_table
.extern     irq_table
.extern     tss


.text

.global     restart_process


# 2 * i8259A interrupt controller
# HARDWARE interrrupt
.global     hw_int00
.global     hw_int01
.global     hw_int02
.global     hw_int03
.global     hw_int04
.global     hw_int05
.global     hw_int06
.global     hw_int07
.global     hw_int08
.global     hw_int09
.global     hw_int10
.global     hw_int11
.global     hw_int12
.global     hw_int13
.global     hw_int14
.global     hw_int15
.global     disable_irq
.global     enable_irq

# SOFTWARE interrupt
.global     sys_call

restart_process:
    mov     process_ready, %esp
    lldt    PROC_LDT_SEL(%esp)
    lea     PROC_STACK_TOP(%esp), %eax
    movl    %eax,       (tss + TSS_ESP0)
restart_re_enter:
    decl    int_reenter
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

    # warning, from now on, we can't use push/pop until 'mov $StackTop, %esp'
    # because it will destroy proc_table

    mov     %edx,   %esi # reserve edx into esi because it store syscall para.
    mov     %ss,    %dx
    mov     %dx,    %ds
    mov     %dx,    %es
    mov     %dx,    %fs
    mov     %esi,   %edx # restore

    mov     %esp,   %esi

    incl    int_reenter
    cmpl    $0,     int_reenter
    jne     _save_1

    mov     $StackTop,   %esp
    pushl    $restart_process
    jmp     *(RET_ADDR_REG-PROC_STACK_BASE)(%esi)
_save_1:
    pushl    $restart_re_enter
    jmp     *(RET_ADDR_REG-PROC_STACK_BASE)(%esi)



# hardware interrupt
.macro master_8259 num
    pushl   $(\num)
    call    dummy_irq_handler
    add     $4,     %esp
    hlt
.endm

.macro slaver_8259 num
    pushl   $(\num)
    call    dummy_irq_handler
    add     $4,     %esp
    hlt
.endm

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

    mov     $\num,  %esi
    call    *irq_table(, %esi, 4)   # irq_table[%esi]
    popl     %ecx
    cli

    # resum clock interrupt
    in      $INT_M_CTRL_MASK, %al
    and     $~(1 << \num),  %al
    out     %al,    $INT_M_CTRL_MASK

    ret
.endm

.align 2
hw_int00:
    hw_int_master 0

hw_int01:
    hw_int_master 1

hw_int02:
    hw_int_master 2

hw_int03:
    hw_int_master 3

hw_int04:
    hw_int_master 4

hw_int05:
    hw_int_master 5

hw_int06:
    hw_int_master 6

hw_int07:
    hw_int_master 7

hw_int08:
    slaver_8259 8

hw_int09:
    slaver_8259 9

hw_int10:
    slaver_8259 10

hw_int11:
    slaver_8259 11

hw_int12:
    slaver_8259 12

hw_int13:
    slaver_8259 13

hw_int14:
    slaver_8259 14

hw_int15:
    slaver_8259 15


disable_irq:
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


enable_irq:
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


# software interrupt
sys_call:
    call    _save
    sti

    push    %esi
    pushl   process_ready

    push    %edx
    push    %ecx
    push    %ebx
    call    *sys_call_table(, %eax, 4)
    add     $(4*4),     %esp

    movl    %eax,       (EAX_REG-PROC_STACK_BASE)(%esi)

    cli
    ret










