/**
    Provide by Walon Li
    2015/5/22
**/

#include "type.h"
#include "pm.h"
#include "string.h"
#include "common.h"

void cstart()
{
    // adjust position
    for (int x = 0 ; x < 13 ; ++x) show_msg("\n") ;

    show_msg("C start...\n") ;

    // Copy Loader's GDT to new GDT
    memcpy(&gdt,                                // new GDT
           (void*)(*(uint32*)(&gdt_ptr[2])),    // base of old GDT
           *((uint16*)(&gdt_ptr[0])) + 1 ) ;    // limit of old GDT
    
    uint16 *gdt_limit = (uint16*)(&gdt_ptr[0]) ;
    uint32 *gdt_base = (uint32*)(&gdt_ptr[2]) ;
    
    // set new limit(GDT 128)
    *gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1 ;
    // set new base
    *gdt_base = (uint32)&gdt ;

    
}
