#include "tester_ipc.h"
#include "tester.h"

#include "disastrOS_constants.h"
#include "disastrOS.h"

#include <stdio.h>
#include <stdlib.h>

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

    printf("Executing utest for resource module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "ipc_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_read, sizeof(utest_read)/sizeof(utest_read[0]), "ipc_read") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_write, sizeof(utest_write)/sizeof(utest_write[0]), "ipc_write") ? 1 : 0;

    return is_all_test_ok;
}

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