#include "tester_ipc.h"
#include "tester.h"

#include "disastrOS_constants.h"
#include "disastrOS.h"

#include <stdio.h>
#include <stdlib.h>


void tester_utest_ipc_utils_sleeper(){
    while(1) {
        getc(stdin);
        disastrOS_printStatus();
    }
    return;
}

int tester_utest_ipc_utils_writer(int resource_id, int count, int expected_write_count, int should_exit, int non_blocking){
    // 0. Initialize
    int return_value, file_descriptor, flags;
    char buffer[1]; // Representative
    
    // 1. Set flags and open resource
    flags = DSOS_O_WRONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "(on writer son) error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(expected_write_count, return_value, "(on writer son) error on ipc write"));
    // 3. If the writer shouldn't unblock we return error and exit from the test
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected return of the son");
        exit(0);
    }
    
    // 4. All ok, return 1
    return 1;
}

int tester_utest_ipc_utils_reader(int resource_id, int count, int expected_read_count, int should_exit, int non_blocking){
    // 0. Initialize
    int return_value, file_descriptor, flags;
    char buffer[1]; // Representative
    
    // 1. Set flags and open resource
    flags = DSOS_O_RDONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "(on read son) error on ipc open"));
    file_descriptor = return_value;

    // 2. Call read
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_read_count, return_value, "(on read son) error on ipc read"));
    // 3. If the reader shouldn't unblock we return error and exit from the test
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected return of the son");
        exit(0);
    }
    
    // 4. All ok, return 1
    return 1;
}