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


void fs_task()
{
    printf("fs task begin\n") ;

    MESSAGE msg ;
    msg.type = MSG_TYPE_DEV_OPEN ;
    send_recv(MSG_BOTH, HDD_TASK, &msg) ;

    while (1) ;
}
