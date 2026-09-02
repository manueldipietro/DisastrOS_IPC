#include "tester.h"

#include "tester_resource.h"
#include "tester_ipc.h"
#include "tester_spawnfd.h"
#include "tester_fifo.h"

#include <stdio.h>

int tester_utest_resources(){
    int is_all_test_ok = 1;
    tester_utest_list utest_mk[] = {
        { .title = "Test disastrOS_resource: mk1: successfully", .utest_fn = tester_resource_mk1},
        { .title = "Test disastrOS_resource: mk2: double make", .utest_fn = tester_resource_mk2},
        { .title = "Test disastrOS_resource: mk3: invalid (negative) or anonymous id", .utest_fn = tester_resource_mk3},
        { .title = "Test disastrOS_resource: mk4: allocating more resource than the memory capacity", .utest_fn = tester_resource_mk4}
    };
    tester_utest_list utest_open[] = {
        { .title = "Test disastrOS_resource: open1: successfully", .utest_fn = tester_resource_open1},
        { .title = "Test disastrOS_resource: open2: invalid (negative) or anonymous id", .utest_fn = tester_resource_open2},
        { .title = "Test disastrOS_resource: open3: unsupported flags", .utest_fn = tester_resource_open3},
        { .title = "Test disastrOS_resource: open4: illegal combination of flags", .utest_fn = tester_resource_open4},
        { .title = "Test disastrOS_resource: open5: non-existing resource", .utest_fn = tester_resource_open5},
        { .title = "Test disastrOS_resource: open6: non-existing resource with DSOS_O_CREATE", .utest_fn = tester_resource_open6},
        { .title = "Test disastrOS_resource: open7: existing resource with DSOS_O_CREATE", .utest_fn = tester_resource_open7},
        { .title = "Test disastrOS_resource: open8: non-existing resource with DSOS_O_CREATE and DSOS_O_EXCL", .utest_fn = tester_resource_open8},
        { .title = "Test disastrOS_resource: open9: existing resource with DSOS_O_CREATE and DSOS_O_EXCL", .utest_fn = tester_resource_open9},
        { .title = "Test disastrOS_resource: open10: multiple open on same resource with RDONLY, WRONLY and RDWR", .utest_fn = tester_resource_open10},
        { .title = "Test disastrOS_resource: open11: create (O_CREATE) more resource than memory capacity", .utest_fn = tester_resource_open11},
        { .title = "Test disastrOS_resource: open12: allocating more descriptor in a process than memory capacity", .utest_fn = tester_resource_open12},
        { .title = "Test disastrOS_resource: open13: allocation more descriptor_ptr in a process than memory capacity", .utest_fn = tester_resource_open13}
    };
    tester_utest_list utest_unlink[] = {
        { .title = "Test disastrOS_resource: unlink1: unlink not open resource", .utest_fn = tester_resource_unlink1},
        { .title = "Test disastrOS_resource: unlink2: unlink open resource", .utest_fn = tester_resource_unlink2},
        { .title = "Test disastrOS_resource: unlink3: invalid id (negative) or anonymous id", .utest_fn = tester_resource_unlink3},
        { .title = "Test disastrOS_resource: unlink4: non-existing resource", .utest_fn = tester_resource_unlink4},
        { .title = "Test disastrOS_resource: unlink5: double unlink", .utest_fn = tester_resource_unlink5},
        { .title = "Test disastrOS_resource: unlink6: try to reuse id immediately after unlink", .utest_fn = tester_resource_unlink6},
    };
    tester_utest_list utest_close[] = {
        { .title = "Test disastrOS_resource: close1: successfully (not unlinked)", .utest_fn = tester_resource_close1},
        { .title = "Test disastrOS_resource: close2: successfully (unlinked)", .utest_fn = tester_resource_close2},
        { .title = "Test disastrOS_resource: close3: double close file descriptor", .utest_fn = tester_resource_close3},
        { .title = "Test disastrOS_resource: close4: invalid file descriptor (negative and never opened)", .utest_fn = tester_resource_close4},
        { .title = "Test disastrOS_resource: close5: close unlinked resource with multiple open descriptors", .utest_fn = tester_resource_close5},
        { .title = "Test disastrOS_resource: close6: automatic close of descriptor on process exit", .utest_fn = tester_resource_close6},
        { .title = "Test disastrOS_resource: close7: automatic close of descriptor on process exit (with unlinked resource)", .utest_fn = tester_resource_close7},  
    };
    tester_utest_list utest_read[] = {
        { .title = "Test disastrOS_resource: read1: invalid file descriptor", .utest_fn = tester_resource_read1},
        { .title = "Test disastrOS_resource: read2: buffer or count invalid", .utest_fn = tester_resource_read2},
        { .title = "Test disastrOS_resource: read3: read operation not supported", .utest_fn = tester_resource_read3},
        { .title = "Test disastrOS_resource: read4: access mode", .utest_fn = tester_resource_read4},
        { .title = "Test disastrOS_resource: read5: test read operation", .utest_fn = tester_resource_read5},
    };
    tester_utest_list utest_write[] = {
        { .title = "Test disastrOS_resource: write1: invalid file descriptor", .utest_fn = tester_resource_write1},
        { .title = "Test disastrOS_resource: write2: buffer or count invalid", .utest_fn = tester_resource_write2},
        { .title = "Test disastrOS_resource: write3: write operation not supported", .utest_fn = tester_resource_write3},
        { .title = "Test disastrOS_resource: write4: access mode", .utest_fn = tester_resource_write4},
        { .title = "Test disastrOS_resource: write5: test write operation", .utest_fn = tester_resource_write5},
    };
    printf("Executing utest for resource module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "resource_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_open, sizeof(utest_open)/sizeof(utest_open[0]), "resource_open") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_unlink, sizeof(utest_unlink)/sizeof(utest_unlink[0]), "resource_unlink") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_close, sizeof(utest_close)/sizeof(utest_close[0]), "resource_close") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_read, sizeof(utest_read)/sizeof(utest_read[0]), "resource_read") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_write, sizeof(utest_write)/sizeof(utest_write[0]), "resource_write") ? 1 : 0;
    return is_all_test_ok;
}

int tester_utest_ipc(){
    int is_all_test_ok = 1;
    tester_utest_list utest_mk[] = {
        { .title = "Test disastrOS_ipc: mk1:", .utest_fn = tester_utest_ipc_mk1},
        { .title = "Test disastrOS_ipc: mk2:", .utest_fn = tester_utest_ipc_mk2},
        { .title = "Test disastrOS_ipc: mk3:", .utest_fn = tester_utest_ipc_mk3},
        { .title = "Test disastrOS_ipc: mk4:", .utest_fn = tester_utest_ipc_mk4}
    };
    tester_utest_list utest_read[] = {
        { .title = "Test disastrOS_ipc: read1: successfully", .utest_fn = tester_utest_ipc_read1},
        { .title = "Test disastrOS_ipc: read2: blocking reader test", .utest_fn = tester_utest_ipc_read2},
        { .title = "Test disastrOS_ipc: read3: unblock after write", .utest_fn = tester_utest_ipc_read3},
        { .title = "Test disastrOS_ipc: read4: double unblock after write", .utest_fn = tester_utest_ipc_read4},
        { .title = "Test disastrOS_ipc: read5: partially read", .utest_fn = tester_utest_ipc_read5},
        { .title = "Test disastrOS_ipc: read6: unblock write", .utest_fn = tester_utest_ipc_read6},
        { .title = "Test disastrOS_ipc: read7: read with count > size_max", .utest_fn = tester_utest_ipc_read7},
        { .title = "Test disastrOS_ipc: read8: non blocking with empty buffer", .utest_fn = tester_utest_ipc_read8},
        { .title = "Test disastrOS_ipc: read9: non blocking with non empty buffer", .utest_fn = tester_utest_ipc_read9},
        { .title = "Test disastrOS_ipc: read10: read with count=0", .utest_fn = tester_utest_ipc_read10}
    };
    tester_utest_list utest_write[] = {
        { .title = "Test disastrOS_ipc: write1: successfully", .utest_fn = tester_utest_ipc_write1},
        { .title = "Test disastrOS_ipc: write2:", .utest_fn = tester_utest_ipc_write2},
        { .title = "Test disastrOS_ipc: write3:", .utest_fn = tester_utest_ipc_write3},
        { .title = "Test disastrOS_ipc: write4:", .utest_fn = tester_utest_ipc_write4},
        { .title = "Test disastrOS_ipc: write5:", .utest_fn = tester_utest_ipc_write5},
        { .title = "Test disastrOS_ipc: write6:", .utest_fn = tester_utest_ipc_write6},
        { .title = "Test disastrOS_ipc: write7:", .utest_fn = tester_utest_ipc_write7},
        { .title = "Test disastrOS_ipc: write8:", .utest_fn = tester_utest_ipc_write8},
        { .title = "Test disastrOS_ipc: write9:", .utest_fn = tester_utest_ipc_write9},
        { .title = "Test disastrOS_ipc: write10:", .utest_fn = tester_utest_ipc_write10}
    };
    printf("Executing utest for ipc module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "ipc_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_read, sizeof(utest_read)/sizeof(utest_read[0]), "ipc_read") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_write, sizeof(utest_write)/sizeof(utest_write[0]), "ipc_write") ? 1 : 0;
    return is_all_test_ok;
}

int tester_utest_spawnfd(){
    int is_all_test_ok = 1;
    tester_utest_list utest_spawnfd[] = {
        { .title = "Test disastrOS_spawn_withfd: test", .utest_fn = tester_utest_spawnfd1}
    };
    printf("Executing utest for disastrOS_spawn_withfd syscall:\n");
    is_all_test_ok *= tester_utest_executelist(utest_spawnfd, sizeof(utest_spawnfd)/sizeof(utest_spawnfd[0]), "disastrOS_spawn_withfd") ? 1 : 0;
    return is_all_test_ok;
}

int tester_utest_circular_buffer(){
    int is_all_test_ok = 1;
    tester_utest_list utest_circular_buffer[] = {
        { .title = "Test circular_buffer 1: write without wrap-around", .utest_fn = tester_utest_circular_buffer_1},
        { .title = "Test circular_buffer 2: read without wrap-around", .utest_fn = tester_utest_circular_buffer_2},
        { .title = "Test circular_buffer 3: write with wrap-around", .utest_fn = tester_utest_circular_buffer_3},
        { .title = "Test circular_buffer 4: read with wrap-around", .utest_fn = tester_utest_circular_buffer_4},
        { .title = "Test circular_buffer 5: write full buffer (from start_pos)", .utest_fn = tester_utest_circular_buffer_5},
        { .title = "Test circular_buffer 6: read full buffer (from start_pos)", .utest_fn = tester_utest_circular_buffer_6},
        { .title = "Test circular_buffer 7: write full buffer (from middle pos)", .utest_fn = tester_utest_circular_buffer_7},
        { .title = "Test circular_buffer 8: read full buffer (from middle pos)", .utest_fn = tester_utest_circular_buffer_8},
    };
    printf("Executing utest for circular buffer:\n");
    is_all_test_ok *= tester_utest_executelist(utest_circular_buffer, sizeof(utest_circular_buffer)/sizeof(utest_circular_buffer[0]), "circular buffer") ? 1 : 0;
    return is_all_test_ok;
}

int tester_utest_fifo(){
    int is_all_test_ok = 1;
    tester_utest_list utest_mk[] = {
        { .title = "Test disastrOS_fifo: mk1:", .utest_fn = tester_utest_fifo_mk1},
        { .title = "Test disastrOS_fifo: mk2:", .utest_fn = tester_utest_fifo_mk2},
        { .title = "Test disastrOS_fifo: mk3:", .utest_fn = tester_utest_fifo_mk3},
        { .title = "Test disastrOS_fifo: mk4:", .utest_fn = tester_utest_fifo_mk4}
    };
    tester_utest_list utest_onopen[] = {
        { .title = "Test disastrOS_fifo: onopen1: invalid flag DSOS_O_RDWR", .utest_fn = tester_utest_fifo_onopen1},
        { .title = "Test disastrOS_fifo: onopen2: blocking reader opener", .utest_fn = tester_utest_fifo_onopen2},
        { .title = "Test disastrOS_fifo: onopen3: unblock readers opener after opener writer", .utest_fn = tester_utest_fifo_onopen3},
        { .title = "Test disastrOS_fifo: onopen4: blocking writer opener", .utest_fn = tester_utest_fifo_onopen4},
        { .title = "Test disastrOS_fifo: onopen5: unblocking writers opener after opener", .utest_fn = tester_utest_fifo_onopen5},
        { .title = "Test disastrOS_fifo: onopen6: DSOS_O_NONBLOCKING readers opener without writers", .utest_fn = tester_utest_fifo_onopen6},
        { .title = "Test disastrOS_fifo: onopen7: DSOS_O_NONBLOCKING writers opener without readers", .utest_fn = tester_utest_fifo_onopen7},
        { .title = "Test disastrOS_fifo: onopen8: DSOS_O_NONBLOCKING readers opener with writers", .utest_fn = tester_utest_fifo_onopen8},
        { .title = "Test disastrOS_fifo: onopen9: DSOS_O_NONBLOCKING writers opener with readers", .utest_fn = tester_utest_fifo_onopen9},
    };
    tester_utest_list utest_readandwrite[] = {
        { .title = "Test disastrOS_fifo: read1: no writer and no more data (EOF return)", .utest_fn = tester_utest_fifo_read1},
        { .title = "Test disastrOS_fifo: write1: no more reader (DSOS_E_PIPE) ", .utest_fn = tester_utest_fifo_write1}
    };
    tester_utest_list utest_onclose[] = {
    };
    tester_utest_list utest_mkpipe[] = {
    };
    printf("Executing utest for fifo module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "fifo_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_onopen, sizeof(utest_onopen)/sizeof(utest_onopen[0]), "fifo_onopen") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_readandwrite, sizeof(utest_readandwrite)/sizeof(utest_readandwrite[0]), "fifo_read and fifo_write") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_onclose, sizeof(utest_onclose)/sizeof(utest_onclose[0]), "fifo_onclose") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_mkpipe, sizeof(utest_mkpipe)/sizeof(utest_mkpipe[0]), "pipe_mk") ? 1 : 0;
    return is_all_test_ok;
}

