#
#   Provide by Walon Li
#   2015/5/16
#


.code32

.global _start

.text

_start:
    mov     $0xf,       %ah
    mov     $'K',       %al
    mov     %ax,        %gs:((80*50+39)*2)
    jmp     .


