#include "tester_aux.h"
#include "tester.h"

#include "disastrOS.h"

#include <stdio.h>

void tester_aux_sleeper(){
    while(1) {getc(stdin);}
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
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "(on writer child) error on ipc open"));
    file_descriptor = return_value;
    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(expected_write_count, return_value, "(on writer child) error on ipc write"));
    // 3. If the writer shouldn't unblock we return error and exit from the test
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected return of the child");
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
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "(on read child) error on ipc open"));
    file_descriptor = return_value;
    // 2. Call read
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_read_count, return_value, "(on read child) error on ipc read"));
    // 3. If the reader shouldn't unblock we return error and exit from the test
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected return of the child");
        exit(0);
    }
    // 4. All ok, return 1
    return 1;
}

int tester_utest_fifo_utils_reader_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval){
    // 0. Initialize
    int return_value, file_descriptor, flags;
    // 1. Set flags and open resource
    flags = DSOS_O_RDONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    file_descriptor = return_value;
    // 2. Check if should exit from syscall
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the child from open (with code %d)", return_value);
        exit(0);
    }
    // 3. Check return value of open
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
    // 2. Check if should exit from syscall
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the son from open (with code %d)", return_value);
        exit(0);
    }
    // 3. Check return value of open
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_open_retval, file_descriptor, "(on writer_open son) error on disastrOS_open"));
    // 4. All ok, return 1
    return 1;
}

int tester_utest_fifo_utils_reader(int resource_id, int count, int expected_read_count, char* buffer, int should_exit, int non_blocking){
    // 0. Initialize
    int return_value, file_descriptor, flags;
    // 1. Set flags and open resource
    flags = DSOS_O_RDONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "(on read child) error on ipc open"));
    file_descriptor = return_value;
    // 2. Call read
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_read_count, return_value, "(on read child) error on ipc read"));
    // 3. If the reader shouldn't unblock we return error and exit from the test
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected return of the child");
        exit(0);
    }
    // 4. All ok, return 1
    return 1;
}

int tester_utest_fifo_utils_writer(int resource_id, int count, int expected_write_count, char* buffer, int should_exit, int non_blocking){
    // 0. Initialize
    int return_value, file_descriptor, flags;
    // 1. Set flags and open resource
    flags = DSOS_O_WRONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "(on writer child) error on ipc open"));
    file_descriptor = return_value;
    // 2. Call write
    disastrOS_sleep(1);
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(expected_write_count, return_value, "(on writer child) error on ipc write"));
    // 3. If the writer shouldn't unblock we return error and exit from the test
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected return of the child");
        exit(0);
    }
    // 4. All ok, return 1
    return 1;
}
