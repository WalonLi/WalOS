/**
    Provide by Walon Li
    2015/03/25
**/

.code16

.text
    mov %cs, %ax
    mov %ax, %ds
    mov %ax, %es
    call ShowMsg
    jmp .           # infinite loop
    

ShowMsg:
    mov $Msg, %ax
    mov %ax, %bp
    mov $MsgLen, %cx

    // for int 10h
    mov $0x1301, %ax
    mov $0x00c, %bx
    mov $0, %dl
    int $0x10
    ret

Msg:    
    .ascii "Hello, Walon OS!"
    MsgLen = . - Msg    
.org 510            #fill null opcode until offset 510
.word 0xaa55        #end mark


