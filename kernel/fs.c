/**
    Provide by Walon Li
    2015/7/1
**/

#include "kernel/process.h"
#include "kernel/interrupt.h"
#include "kernel/hdd.h"
#include "kernel/io.h"
#include "kernel/global.h"
#include "lib/string.h"
#include "lib/debug.h"


void fs_task()
{
    printf("fs task begin\n") ;

    MESSAGE msg ;
    msg.type = MSG_DEV_OPEN ;
    msg.u.m3.m3i4 = DEV_MINOR(ROOT_DEV) ;
    send_recv(MSG_BOTH, drive_map[DEV_MAJOR(ROOT_DEV)].drive_number, &msg) ;

    while (1) ;
}
