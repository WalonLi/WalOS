/**
    Provide by Walon Li
    2015/6/26
**/


#include "kernel/message.h"

void block(PROCESS *proc)
{
    ASSERT(proc->flags) ;
}

void unblock(PROCESS *proc)
{
    ASSERT(!proc->flags) ;
}

int msg_send(PROCESS *proc, int src_dest, MESSAGE *msg)
{
    return 0 ;
}

int msg_recv(PROCESS *proc, int src_dest, MESSAGE *msg)
{
    return 0 ;
}

int msg_both(PROCESS *proc, int src_dest, MESSAGE *msg)
{
    return 0 ;
}
