#
#   Provide by Walon Li
#   2015/5/16
#


.code32

.extern     cstart
.extern     gdt_pdr


.bss
#.lcomm    StackSpace,     1024*2
StackSpace:     .space      1024*2,        0
StackTop:


.text

.global     _start

_start:
    mov     $0xf,       %ah
    mov     $'K',       %al
    mov     %ax,        %gs:((80*21+39)*2)

    mov     $StackTop,   %esp
    sgdtw   gdt_ptr
    call    cstart
    lgdtw   gdt_ptr

    jmp     .



