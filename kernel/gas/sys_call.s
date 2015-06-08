#
#   Provide by Walon Li
#   2015/6/8
#
.include "kernel/gas/macro.inc"

.code32


.text

.global     get_ticks

get_ticks:
    mov     $NUM_GET_TICKS, %eax
    int     $INT_VECTOR_SYS_CALL
    ret



