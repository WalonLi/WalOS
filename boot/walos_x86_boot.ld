/**
    Provide by Walon Li
    2015/03/25
**/

SECTIONS
{
    /* Programe will be loaded to 0x7c00 */
    . = 0x7c00;
    .text :
    {
        _ftext = .;
    } = 0
}

