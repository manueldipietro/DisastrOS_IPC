#pragma once
#include "disastrOS_pcb.h"

/*
  Global variables used by disastrOS

*/

extern PCB* init_pcb; // pcb of the init process
extern PCB* running;  // pcb of the currently running process
extern int last_pid;  // last pid to be generates
extern ListHead ready_list; // list of the ready processes
extern ListHead waiting_list; // list of the waiting processes
extern ListHead zombie_list;  // zombies
extern ListHead resources_list; // resources

extern ListHead timer_list;     //timers (concrete)
extern volatile int disastrOS_time; //global time of disastrOS

// a resource can be a device, a file or an ipc thing

typedef void(*SyscallFunctionType)();


// these are used for "trapping" in protected mode
extern ucontext_t main_context;

// this is used for shutting down (set it to 1, and return the control to main)
extern int shutdown_now;

// Global variable identifying the last ID used by anonymous resources.
// We are ignoring the ID reuse issue for now.
// To support anonymous resources, we divide the resource ID space into two subspaces:
// 0 to DSOS_ANON_RES_STARTID-1 for user-named resources, and DSOS_ANON_RES_STARTID
// onwards for anonymous resources automatically allocated by the kernel.
extern int dsos_last_anon_resource_id;

