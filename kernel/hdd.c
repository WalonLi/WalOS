/**
    Provide by Walon Li
    2015/7/1
**/

#include "kernel/process.h"
#include "kernel/interrupt.h"
#include "kernel/hdd.h"
#include "kernel/io.h"
#include "lib/string.h"
#include "lib/debug.h"

static uint8_t hdd_status ;

void hdd_task()
{
    MESSAGE msg ;

    hdd_init() ;
    while (true)
    {
        send_recv(MSG_RECEIVE, P_ANY, &msg) ;
        int src = msg.source ;

        switch (msg.type)
        {
        case MSG_TYPE_DEV_OPEN:
            hdd_identify() ;
            break ;
        default:
            CRITICAL("hdd_task") ;
            break ;
        }
        send_recv(MSG_SEND, src, &msg) ;
    }
}

void hdd_init()
{
    uint8_t *drives = (uint8_t*) 0x475 ;
    printf("drives:%d \n", *drives) ;

    ASSERT(*drives) ; // if no hdd, throw assert

    set_irq_handler(IRQ_AT, hdd_int_handler) ;
    enable_irq(IRQ_M_TO_S) ;
    enable_irq(IRQ_AT) ;
}

// hdd interrupt
void hdd_int_handler(int irq)
{
    hdd_status = inb(HDD_REG_STATUS) ;
    deliver_int_to_proc(HDD_TASK) ;
}

void hdd_identify()
{

}
