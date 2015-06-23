#
#   Provide by Walon Li
#   2015/6/8
#
.include "kernel/gas/macro.inc"

.code32


.text

.global     get_ticks
.global     write

get_ticks:
    mov     $GET_TICKS_INDEX, %eax
    int     $INT_VECTOR_SYS_CALL
    ret

write:
    mov     $WRITE_INDEX,  %eax
    mov     4(%esp),    %ebx
    mov     8(%esp),    %ecx
    int     $INT_VECTOR_SYS_CALL
    ret



