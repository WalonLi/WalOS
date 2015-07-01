/**
    Provide by Walon Li
    2015/6/25
**/
#include "kernel/global.h"
#include "kernel/process.h"
#include "lib/common.h"
#include "lib/string.h"
#include "lib/debug.h"

void sys_task()
{
    MESSAGE msg ;
    while(true)
    {
        send_recv(MSG_RECEIVE, P_ANY, &msg) ;
        int src = msg.source ;

        switch (msg.type)
        {
        case MSG_TYPE_GET_TICKS:
            // u.m3.m3i1 = Return value;
            msg.u.m3.m3i1 = ticks ;
            send_recv(MSG_SEND, src, &msg) ;
            break ;
        default:
            CRITICAL("sys_task") ;
            break ;
        }
    }
}
