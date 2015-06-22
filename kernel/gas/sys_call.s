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
    mov     $NUM_GET_TICKS, %eax
    int     $INT_VECTOR_SYS_CALL
    ret

write:
    mov     $1,         %eax
    mov     4(%esp),    %ebx
    mov     8(%esp),    %ecx
    int     $INT_VECTOR_SYS_CALL
    ret



