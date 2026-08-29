#include "tester_fifo.h"
#include "tester.h"
#include "tester_aux.h"

#include "disastrOS_constants.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        { .title = "Test disastrOS_fifo: mk1:", .utest_fn = tester_utest_fifo_read1},
    };


    printf("Executing utest for resource module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "fifo_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_onopen, sizeof(utest_onopen)/sizeof(utest_onopen[0]), "fifo_onopen") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_readandwrite, sizeof(utest_readandwrite)/sizeof(utest_readandwrite[0]), "fifo_read and fifo_write") ? 1 : 0;

    return is_all_test_ok;
}

void tester_utest_fifo_utils_sleeper(){
    while(1) {
        getc(stdin);
    }
    return;
}

int tester_utest_fifo_utils_reader_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval){
    // 0. Initialize
    int return_value, file_descriptor, flags;

    // 1. Set flags and open resource
    flags = DSOS_O_RDONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    file_descriptor = return_value;

    // 2. 
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the son from open (with code %d)", return_value);
        exit(0);
    }

    // 3.
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_open_retval, file_descriptor, "(on writer_open son) error on disastrOS_open"));

    // 4. All ok, return 1
    return 1;
}

int tester_utest_fifo_utils_writer_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval){
    // 0. Initialize
    int return_value, file_descriptor, flags;

    // 1. Set flags and open resource
    flags = DSOS_O_WRONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    file_descriptor = return_value;

    // 2. 
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the son from open (with code %d)", return_value);
        exit(0);
    }

    // 3.
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_open_retval, file_descriptor, "(on writer_open son) error on disastrOS_open"));

    // 4. All ok, return 1
    return 1;
}
