/**
    Provide by Walon Li
    2015/6/25
**/
#include "kernel/global.h"
#include "kernel/process.h"
#include "lib/common.h"
#include "lib/string.h"
#include "kernel/message.h"
#include "lib/debug.h"

extern int send_recv(int function, int src_dest, MESSAGE* p_msg);
void sys_task()
{
    MESSAGE msg ;
    while(true)
    {
        p_send_recv(IPC_RECEIVE, P_ANY, &msg) ;
        int src = msg.source ;

        switch (msg.type)
        {
        case GET_TICKS:
            msg.RETVAL = ticks ;
            p_send_recv(IPC_SEND, src, &msg) ;
            break ;
        default:
            CRITICAL("fuck....") ;
            break ;
        }
    }
}
