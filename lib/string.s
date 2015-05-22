#
#   Provide by Walon Li
#   2015/5/22
#

.code32

.text

.global     memcpy

# void *MemCpy(void* es:dest, ds:src, int size)
memcpy:
    push    %ebp
    mov     %esp,           %ebp

    push    %esi
    push    %edi
    push    %ecx

    movl    8(%ebp),        %edi # dest
    movl    12(%ebp),       %esi # src
    movl    16(%ebp),       %ecx # counter

copy_ch:
    cmp     $0,             %ecx
    je      copy_end

    movb    %ds:(%esi),     %al
    inc     %esi
    mov     %al,            %es:(%edi)
    inc     %edi
    dec     %ecx
    jmp     copy_ch
copy_end:
    movl    8(%ebp),        %eax

    pop     %ecx
    pop     %edi
    pop     %esi
    mov     %ebp,           %esp
    pop     %ebp
    ret

