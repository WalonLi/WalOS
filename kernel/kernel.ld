/**
    Provide by Walon Li
    2015/05/25
**/

SECTIONS
{
    . = 0x30400 ;
    .text :
    {
        /**
        *(.text)
        *(.text.*)
        *(.eh_frame)
        *(.rodata)
        *(.rodata*)

        _sidata = . ;
        **/
        _ftext = .;
    }
    /**
    . = 0x40000 ;
    
    .data : AT(0x40000)
    {
        _fdata = . ;
    }
    **/
    /**
    *(.data)
    .bss :
    {
        _fbss = .;
    }
    **/
}