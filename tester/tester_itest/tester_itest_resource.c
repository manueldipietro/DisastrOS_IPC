#include "tester_resource.h"
#include "tester_aux.h"

#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include <stdio.h>

void test_itest_resource1_child(){
    // 1. Open the resource 0 and resource disastrOS_getpid - 1
    printf(TESTER_CHILD_TAG_WID "hello, I am the child function. I will iterate a bit, before terminating\n",disastrOS_getpid());
    int flags = DSOS_O_RDWR;
    int fd1 = disastrOS_open(0, DSOS_O_RDONLY);
    if(fd1 < 0){
        printf(TESTER_CHILD_TAG_WID "error while opening resource 0, exiting\n", disastrOS_getpid());
        disastrOS_exit(-1);
    }
    int fd2 = disastrOS_open(disastrOS_getpid()-1, flags);
    if(fd2 < 0){
        printf(TESTER_CHILD_TAG_WID "error while opening resource %d, exiting\n", disastrOS_getpid(), disastrOS_getpid()-1);
        disastrOS_exit(-1);
    }
    printf(TESTER_CHILD_TAG_WID " after open, the system status is:\n", disastrOS_getpid());
    disastrOS_printStatus();
    // 2. Iterating a while
    for (int i=0; i<(disastrOS_getpid()+1); ++i){
        printf(TESTER_CHILD_TAG_WID "iterate %d\n", disastrOS_getpid(), i);
        disastrOS_sleep((20-disastrOS_getpid())*2);
    }
    // 3. Return
    disastrOS_exit(disastrOS_getpid());
}

void test_itest_resource1_init(){
    // 0. Print test header
    printf("----------------------------------------------------------------------------------------------\n");
    printf(TESTER_ANSI_BRIGHT_RED TESTER_ANSI_BOLD "INTEGRATION TEST RESOURCES\n" TESTER_ANSI_RESET);
    printf("Init will create and open 11 resource, and will spawn 10 childrens\n");
    printf("Each child open resource 0 and i-th (from 1 to 10) resource");
    printf("When child exit the resource will be closed automatically by syscall exit\n");
    printf("When init exit the resource will be closed manually and even resource unliked\n");
    printf("----------------------------------------------------------------------------------------------\n");
    // 1. Spawn the sleeper process
    disastrOS_spawn(tester_aux_sleeper, 0);
    // 2. Create resource and spawn sons
    printf(TESTER_INIT_TAG "i'm spawning threads and creating resource\n");
    int alive_children=0;
    for (int i = 0; i < 11; ++i) {
        int flags=DSOS_O_CREAT | DSOS_O_RDWR;
        int fd=disastrOS_open(i, flags);
        if(fd<0){
            printf(TESTER_INIT_TAG "error while opening resource %d, exiting\n", i);
            return;
        }
        if(i != 0){
            disastrOS_spawn(test_itest_resource1_child, 0);
            alive_children++;
        }
    }
    printf(TESTER_INIT_TAG "after child spawn, the system status is:\n");
    disastrOS_printStatus();
    // 3. Wait for sons
    int retval, pid;
    while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
        disastrOS_printStatus();
        printf(TESTER_INIT_TAG "child: %d terminated, retval: %d, alive: %d \n", pid, retval, alive_children);
        --alive_children;
    }
    printf(TESTER_INIT_TAG "after all child exit, the system status is:\n");
    disastrOS_printStatus();
    // 4. Unlink even resources
    printf(TESTER_INIT_TAG "all childern exited, i will close all resource and unlink even resource and close all resource\n");
    for (int i=0; i<11; ++i){
        if(i%2==0){
            retval = disastrOS_unlink(i);
            if(retval != 0){
                printf(TESTER_INIT_TAG "error while unlinking resource %d, exiting\n", i);
                return;
            }
        }
        retval = disastrOS_close(i);
        if(retval != 0){
            printf(TESTER_INIT_TAG "error while closing resource %d, exiting\n", i);
            return;
        }
    }
    printf(TESTER_INIT_TAG "after close and unlink even resource, the system status is:\n");
    disastrOS_printStatus();
    // 5. Return
    return;
}