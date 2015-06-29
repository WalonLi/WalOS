/**
    Provide by Walon Li
    2015/6/26
**/


#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "type.h"
#include "lib/debug.h"
#include "kernel/process.h"

/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	uint64_t	m3l1;
	uint64_t	m3l2;
	void*	m3p1;
	void*	m3p2;
};

typedef struct _MESSAGE{
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;

#define	RETVAL		u.m3.m3i1

#define IPC_SEND    0x0
#define IPC_RECEIVE 0x1
#define IPC_BOTH    0x2 // not support

#define MSG_SENDING 0x2
#define MSG_RECVING 0x4

int msg_send(PROCESS *p_send, int dest, MESSAGE *msg) ;
int msg_recv(PROCESS *p_recv, int src, MESSAGE *msg) ;
int msg_both(PROCESS *proc, int src_dest, MESSAGE *msg) ;
bool dead_lock(int src, int dest) ;

int p_send_recv(int func, int src_dest, MESSAGE *msg) ;

enum msgtype {
	/*
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,
};

#endif // __MESSAGE_H__
