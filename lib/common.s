#
#   Provide by Walon Li
#   2015/5/23
#


.code32

.text
.global     show_msg
.global     wb_port
.global     rb_port

_position:  .4byte      0

# void show_msg( char *msg )
show_msg:
    push    %ebp
    mov     %esp,       %ebp

    movl    8(%ebp),    %esi
    movl    _position,  %edi
    mov     $0xf,       %ah

msg_1:
    lodsb
    test    %al,        %al
    jz      msg_2
    cmp     $0xa,       %al     # is enter?
    jnz     msg_3
    push    %eax
    mov     %edi,       %eax
    mov     $160,       %bl
    div     %bl
    and     $0xff,      %eax
    inc     %eax
    mov     $160,       %bl
    mul     %bl
    mov     %eax,       %edi
    pop     %eax
    jmp     msg_1
msg_3:
    mov     %ax,        %gs:(%edi)
    add     $2,         %edi
    jmp     msg_1

msg_2:
    movl    %edi,       _position

    pop     %ebp
    ret


#void wb_port(uint16_t port, uint8_t value) ;
wb_port:
    mov     4(%esp),    %edx
    mov     8(%esp),    %al
    out     %al,        %dx
    nop     #small dely
    nop
    nop
    ret

#uint8_t rb_port(uint16_t port) ;
rb_port:
    mov     4(%esp),    %edx
    xor     %eax,       %eax
    in      %dx,        %al
    nop     #small dely
    nop
    nop
    ret
    

