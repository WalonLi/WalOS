/**
    Provide by Walon Li
    2015/03/29
**/

.code16
.text
    mov $0xb800, %ax            // video segment
    mov %ax, %gs
    mov $0xC, %ah
    mov $'L', %al
    mov %ax, %gs:((80*0+39)*2)  // col:0 row:39
    jmp .
    
