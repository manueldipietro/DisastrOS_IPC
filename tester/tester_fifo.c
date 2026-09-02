#include "tester_fifo.h"
#include "tester.h"
#include "tester_aux.h"

#include "disastrOS_constants.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // 2. If the child ter
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the child from open (with code %d)", return_value);
        exit(0);
    }

    // 3.
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_open_retval, file_descriptor, "(on writer_open child) error on disastrOS_open"));

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
