#pragma once

#define MAX_NUM_PROCESSES 1024
#define MAX_NUM_RESOURCES 1024
#define MAX_NUM_RESOURCES_PER_PROCESS 32
#define MAX_NUM_DESCRIPTORS_PER_PROCESS 32
#define DSOS_ANON_RES_STARTID 512

#define STACK_SIZE        16384
// signals
#define MAX_SIGNALS 32
#define DSOS_SIGCHLD 0x1
#define DSOS_SIGHUP  0x2

// errors
#define DSOS_ESYSCALL_ARGUMENT_OUT_OF_BOUNDS -1
#define DSOS_ESYSCALL_NOT_IMPLEMENTED -2
#define DSOS_ESYSCALL_OUT_OF_RANGE -3
#define DSOS_EFORK  -4
#define DSOS_EWAIT  -5
#define DSOS_ESPAWN  -6
#define DSOS_ESLEEP  -7
#define DSOS_ERESOURCECREATE -8
#define DSOS_ERESOURCEOPEN -9
#define DSOS_ERESOURCENOEXCL -10
#define DSOS_ERESOURCENOFD -11
#define DSOS_ERESOURCECLOSE -12
#define DSOS_ERESOURCEINUSE -13
//---------NEW-ERRORS-CODE-TO-CHECK
#define DSOS_EINVAL         -20
#define DSOS_EXIST          -21
#define DSOS_ENOENT         -22
#define DSOS_ENOMEM         -23

// syscall numbers
#define DSOS_MAX_SYSCALLS 32
#define DSOS_MAX_SYSCALLS_ARGS 8
#define DSOS_CALL_PREEMPT   1
#define DSOS_CALL_FORK      2
#define DSOS_CALL_WAIT      3
#define DSOS_CALL_EXIT      4
#define DSOS_CALL_SPAWN     5
#define DSOS_CALL_SLEEP     6
#define DSOS_CALL_OPEN_RESOURCE 7
#define DSOS_CALL_CLOSE_RESOURCE 8
#define DSOS_CALL_DESTROY_RESOURCE 9
#define DSOS_CALL_SHUTDOWN  10

// New error's code


//resources
/**
    TODO: Togliere questi e portarli tutti sui nuovi flags (quando si aggiorna la open). 
*/
#define DSOS_CREATE 0x1
#define DSOS_READ 0x2
#define DSOS_WRITE 0x3
#define DSOS_EXCL 0x4

// scheduling
#define ALPHA 0.5f
#define INTERVAL 100 // milliseconds for timer tick


// Resources type
#define DSOS_RESTYPE_UNDEFIN    0
#define DSOS_RESTYPE_IPCBASE    1
#define DSOS_RESTYPE_IPCPIPE    2
#define DSOS_RESTYPE_IPCFIFO    3
#define DSOS_RESTYPE_IPCMQAN    4
#define DSOS_RESTYPE_IPCMQNM    5

// Flags for open syscall
#define DSOS_O_RDONLY       0b00000001    
#define DSOS_O_WRONLY       0b00000010
#define DSOS_O_RDWR         0b00000011
#define DSOS_O_CREAT        0b00000100
#define DSOS_O_EXCL         0b00001000
#define DSOS_O_NONBLOCK     0b00010000
