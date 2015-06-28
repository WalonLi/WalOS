/**
    Provide by Walon Li
    2015/6/26
**/


#include "kernel/message.h"
#include "kernel/global.h"
#include "lib/string.h"

extern void process_schedule() ;
void block(PROCESS *proc)
{
    ASSERT(proc->flags) ;
    process_schedule() ;
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
        // dest is waiting for message
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

enum msgtype {
	/*
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,
};

#define INTERRUPT -10
#define INVALID_DRIVER -20
int msg_recv(PROCESS *p_recv, int src, MESSAGE *msg)
{
    ASSERT(p_recv->id != src) ;


    if (p_recv->have_int_msg && (src == P_ANY || src == INTERRUPT))
    {
        // handle interrupt message
        MESSAGE int_msg ;
        memset(&int_msg, '\0', sizeof(MESSAGE)) ;
        int_msg.source = INTERRUPT ;
        int_msg.type = HARD_INT ;

        memcpy(vir_to_linear(p_recv->id, msg), &int_msg, sizeof(MESSAGE)) ;
        p_recv->have_int_msg = false;
    }

    return 0 ;
}

int msg_both(PROCESS *proc, int src_dest, MESSAGE *msg)
{
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
