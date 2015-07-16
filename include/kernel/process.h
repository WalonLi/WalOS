/**
    Provide by Walon Li
    2015/5/31
**/

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "type.h"
#include "kernel/core.h"


typedef struct _MESSAGE MESSAGE ;
typedef struct _PROCESS PROCESS ;

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

struct _MESSAGE{
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
};

// #define	RETVAL		u.m3.m3i1

#define MSG_SEND    0x0
#define MSG_RECEIVE 0x1
#define MSG_BOTH    0x2

// Process Flags
#define MSG_SENDING 0x2
#define MSG_RECVING 0x4

void block(PROCESS *proc) ;
void unblock(PROCESS *proc) ;
int msg_send(PROCESS *p_send, int dest, MESSAGE *msg) ;
int msg_recv(PROCESS *p_recv, int src, MESSAGE *msg) ;
int msg_both(PROCESS *proc, int src_dest, MESSAGE *msg) ;
bool dead_lock(int src, int dest) ;

int send_recv(int func, int src_dest, MESSAGE *msg) ;

#define MSG_SOURCE_INTERRUPT    -10
#define MSG_SOURCE_INVALID_DRIVER  -20
enum msg_type {
	// when hard interrupt occurs, a msg (with type==HARD_INT) will
    // be sent to some tasks
	MSG_TYPE_HW_INTERRUPT = 1,

	// System task
	MSG_TYPE_GET_TICKS,
	MSG_TYPE_DEV_OPEN,
};


typedef void (*task_handler)() ;

typedef struct _REG_STACK
{
    uint32_t gs ;
    uint32_t fs ;
    uint32_t es ;
    uint32_t ds ;
    uint32_t edi ;
    uint32_t esi ;
    uint32_t ebp ;
    uint32_t krnl_esp ;
    uint32_t ebx ;
    uint32_t edx ;
    uint32_t ecx ;
    uint32_t eax ;
    uint32_t ret_addr ;
    uint32_t eip ;
    uint32_t cs ;
    uint32_t eflags ;
    uint32_t esp ;
    uint32_t ss ;
} REG_STACK;

struct _PROCESS
{
    REG_STACK       regs ;
    uint16_t        ldt_sel;
    DESCRIPTOR      ldt[LDT_SIZE] ;
    int             ticks ;
    int             priority ;
    uint32_t        id ;
    char            name[16] ;
    int             console_id ;

    // message
    // new
    int             flags ;
    MESSAGE         *msg ;
    int             recv_from ;
    int             send_to ;
    bool            have_int_msg ;

    // Have a send queue to handle message.
    PROCESS         *current_send ;
    PROCESS         *next_send ;
};

typedef struct _TASK
{
    task_handler    init_eip ;
    int             stack_size ;
    char            name[32] ;
} TASK ;


#define RING0_TASK_CNT      0

#define KRNL_TASK_CNT       4
#define RING1_TASK_CNT      KRNL_TASK_CNT + 0

#define RING2_TASK_CNT      0

#define USR_PROCS_CNT       3
#define RING3_TASK_CNT      USR_PROCS_CNT + 0

#define TOTAL_TASK_CNT      (RING0_TASK_CNT + RING1_TASK_CNT + RING2_TASK_CNT + RING3_TASK_CNT)

#define STACK_SIZE_CONSOLE  0x8000
#define STACK_SIZE_SYS      0x8000
#define STACK_SIZE_HDD      0x8000
#define STACK_SIZE_FS       0x8000
#define STACK_SIZE_PROC_A   0x8000
#define STACK_SIZE_PROC_B   0x8000
#define STACK_SIZE_PROC_C   0x8000
#define STACK_SIZE          STACK_SIZE_PROC_A + STACK_SIZE_PROC_B + STACK_SIZE_PROC_C \
                            + STACK_SIZE_CONSOLE + STACK_SIZE_SYS + STACK_SIZE_HDD + STACK_SIZE_FS
//#define STACK_SIZE          STACK_SIZE_PROC_A


#define GET_PROCESS_ID(p)   (p-proc_table)
#define P_ANY               (TOTAL_TASK_CNT+100)
#define P_NO_TASK           (TOTAL_TASK_CNT+200)

#define CONSOLE_TASK        0x0
#define SYSTEM_TASK         0x1
#define HDD_TASK            0x2
#define FS_TASK             0x3

void init_process_main() ;
void process_schedule() ;
void restart_process() ;
int ldt_linear_addr(PROCESS *p, int index) ;
void* vir_to_linear(int pid, void* vir_addr) ;
void deliver_int_to_proc(int n_task) ;


#endif

