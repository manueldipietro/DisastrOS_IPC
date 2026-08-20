#include "tester_ipc.h"
#include "tester.h"

#include <stdio.h>

int tester_utest_ipc(){
    int is_all_test_ok = 1;

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
    is_all_test_ok *= tester_utest_executelist(utest_write, sizeof(utest_write)/sizeof(utest_write[0]), "resource_write") ? 1 : 0;

    return is_all_test_ok;
}