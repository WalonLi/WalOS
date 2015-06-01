/**
    Provide by Walon Li
    2015/05/25
**/

SECTIONS
{
    . = 0x30400 ;
    .text :
    {
        _ftext = .;
    } = 0 
    
    /**
    . = 0x30400 ;
    .text :
    {
        _ftext = .;
       
        *(.text)
        *(.text.*)
        *(.eh_frame)
        *(.rodata)
        *(.rodata*)
        
    } = 0
    _etext = . ;
    PROVIDE (etext = .);

    .data : 
    {
        _fdata = . ;
        *(.data)
        *(.data.*)
    } = 0
    _edata = . ;
    PROVIDE (_edata = .);

    .bss :
    {
        _fbss = .;
        __bss_start = .;
        *(.bss)
        *(COMMON)
    } = 0
    . = ALIGN(16);
    __bss_end = .;
    _end = .;__end = .; end = .; 
    PROVIDE (end = .);

    **/
    /**
    *(.data)
    .bss :
    {
        _fbss = .;
    }
    **/
}