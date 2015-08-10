/**
    Provide by Walon Li
    2015/6/26
**/


#include "kernel/process.h"
#include "kernel/global.h"
#include "lib/string.h"
#include "lib/debug.h"

extern void process_schedule() ;
void block(PROCESS *proc)
{
    ASSERT(proc->flags) ;
    process_schedule() ; // re schedule
}

void unblock(PROCESS *proc)
{
    ASSERT(!proc->flags) ;
}


int msg_send(PROCESS *p_send, int dest, MESSAGE *msg)
{
    ASSERT(p_send->id != (uint32_t)dest) ;

    PROCESS *p_dest = &proc_table[dest] ;
    if (dead_lock(p_send->id, dest))
        CRITICAL("DEAD LOCK %s %s", p_send->name, p_dest->name) ;


    if ((p_dest->flags & MSG_RECVING) && (p_dest->recv_from == p_send->id || p_dest->recv_from == P_ANY))
    {
        // send message directly.
        ASSERT(p_dest->msg) ;
        ASSERT(msg) ;

        memcpy(vir_to_linear(dest, p_dest->msg), vir_to_linear(p_send->id, msg), sizeof(MESSAGE)) ;
        p_dest->msg = NULL ;
        p_dest->flags &= ~MSG_RECVING ;
        p_dest->recv_from = P_NO_TASK ;
        unblock(p_dest) ;
    }
    else
    {
        // dest is NOT waiting for message
        p_send->flags |= MSG_SENDING ;
        p_send->send_to = dest ;
        p_send->msg = msg ;

        // add to queue
        if (p_dest->current_send)
        {
            for (PROCESS *p = p_dest->current_send ; ; p = p->next_send)
                if (!p->next_send)
                {
                    p->next_send = p_send ;
                    break ;
                }
        }
        else
        {
            p_dest->current_send = p_send ;
        }
        p_send->next_send = NULL ;
        block(p_send) ;
    }

    return 0 ;
}

int msg_recv(PROCESS *p_recv, int src, MESSAGE *msg)
{
    bool copy = false ;
    PROCESS *p_src = null, *p_prev = null;

    ASSERT(p_recv->id != src) ;


    if (p_recv->have_int_msg && (src == P_ANY || src == MSG_SOURCE_INTERRUPT))
    {
        // some interrupt need IPC to transfer message
        // so handle hardware interrupt message
        MESSAGE int_msg ;
        memset(&int_msg, '\0', sizeof(MESSAGE)) ;
        int_msg.source = MSG_SOURCE_INTERRUPT ;
        int_msg.type = MSG_HW_INTERRUPT ;

        memcpy(vir_to_linear(p_recv->id, msg), &int_msg, sizeof(MESSAGE)) ;
        p_recv->have_int_msg = false;
        return 0 ;
    }

    if (src == P_ANY)
    {
        if (p_recv->current_send)
        {
            // get message process by queue
            p_src = p_recv->current_send ;
            copy = true ;
        }
    }
    else
    {
        p_src = &proc_table[src] ;
        if ((p_src->flags & MSG_SENDING) && (p_src->send_to == p_recv->id))
        {
            copy = true ;

            ASSERT(p_recv->current_send) ;
            for (PROCESS *p = p_recv->current_send ; p ; p = p->next_send)
            {
                if (p->id == src)
                {
                    p_src = p ;
                    break ;
                }
                p_prev = p ;
            }
        }
    }

    if (copy)
    {
        // prepare next send into queue
        if (p_src == p_recv->current_send)
            p_recv->current_send = p_src->next_send ;
        else
            p_prev->next_send = p_src->next_send ;

        p_src->next_send = null ;


        memcpy(vir_to_linear(p_recv->id, msg), vir_to_linear(p_src->id, p_src->msg), sizeof(MESSAGE)) ;
        p_src->msg = null ;
        p_src->send_to = P_NO_TASK ;
        p_src->flags &= ~MSG_SENDING ;
        unblock(p_src) ;
    }
    else
    {
        // nobody sending meesage, so restore it.
        p_recv->flags |= MSG_RECVING ;
        p_recv->msg = msg ;
        p_recv->recv_from = src ;
        // p_recv->recv_from = (src == P_ANY) ? P_ANY : p_src->id ;
        block(p_recv) ;
    }
    return 0 ;
}

int msg_both(PROCESS *proc, int src_dest, MESSAGE *msg)
{
    // not support on message.c
    return 0 ;
}

/* To check dead lock(each process dependency)
*  A->B->C->A
*/
bool dead_lock(int src, int dest)
{
    ASSERT(src != dest) ;

    PROCESS *proc = &proc_table[dest] ;
    while (true)
    {
        if (proc->flags & MSG_SENDING)
        {
            if (proc->send_to == src)
            {
                // DEAD LOCK (ex: A->B->C->A)
                // below code will print process name with DEAD LOCK
                for (proc = &proc_table[dest] ; proc != &proc_table[src] ; proc = &proc_table[proc->send_to] )
                {
                    printf("%s->", proc->name) ;
                    ASSERT(proc->msg) ;
                }
                printf("%s->", proc->name) ;
                return true ;
            }
            proc = &proc_table[proc->send_to] ;
        }
        else
        {
            break ;
        }
    }
    return false ;
}

/*
    send_recv will call as_send_recv, this function enhance function.
    and also avoid to use as_send_recv directly.
*/
int	as_send_recv(int function, int src_dest, MESSAGE* p_msg);
int send_recv(int func, int src_dest, MESSAGE *msg)
{
    int ret = 0 ;

    if (func == MSG_RECEIVE)
        memset(msg, 0, sizeof(struct _MESSAGE)) ;

    switch (func)
    {
    case MSG_BOTH:
        ret = as_send_recv(MSG_SEND, src_dest, msg) ;
        if (ret == 0) ret = as_send_recv(MSG_RECEIVE, src_dest, msg) ;
        break ;
    case MSG_SEND:
    case MSG_RECEIVE:
        ret = as_send_recv(func, src_dest, msg) ;
        break ;
    default:
        break ;
    }
    return 0 ;
}
