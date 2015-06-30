#
#   Provide by Walon Li
#   2015/6/8
#
.include "kernel/gas/macro.inc"

.code32


.text

.global     as_printx
.global     as_send_recv

#get_ticks:
#    mov     $GET_TICKS_INDEX, %eax
#    int     $INT_VECTOR_SYS_CALL
#    ret

#write:
#    mov     $WRITE_INDEX,  %eax
#    mov     4(%esp),    %ebx
#    mov     8(%esp),    %ecx
#    int     $INT_VECTOR_SYS_CALL
#    ret

as_printx:
    mov     $PRINTX_INDEX, %eax
    mov     4(%esp),    %edx
    int     $INT_VECTOR_SYS_CALL
    ret

as_send_recv:
    mov     $SEND_RECV_INDEX, %eax
    mov     4(%esp),    %ebx            #function
    mov     8(%esp),    %ecx            #src_dest
    mov     12(%esp),   %edx            #msg
    int     $INT_VECTOR_SYS_CALL
    ret



