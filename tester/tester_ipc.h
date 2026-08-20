#pragma once

int tester_utest_ipc();

int tester_utestipc_mk1(char* test_name);
int tester_utestipc_mk2(char* test_name);
int tester_utestipc_mk3(char* test_name);
int tester_utestipc_mk4(char* test_name);

int tester_utest_ipc_read1(char* test_name);
int tester_utest_ipc_read2(char* test_name);
int tester_utest_ipc_read3(char* test_name);
int tester_utest_ipc_read4(char* test_name);
int tester_utest_ipc_read5(char* test_name);
int tester_utest_ipc_read6(char* test_name);
int tester_utest_ipc_read7(char* test_name);
int tester_utest_ipc_read8(char* test_name);
int tester_utest_ipc_read9(char* test_name);
int tester_utest_ipc_read10(char* test_name);

int tester_utest_ipc_write1(char* test_name);
int tester_utest_ipc_write2(char* test_name);
int tester_utest_ipc_write3(char* test_name);
int tester_utest_ipc_write4(char* test_name);
int tester_utest_ipc_write5(char* test_name);
int tester_utest_ipc_write6(char* test_name);
int tester_utest_ipc_write7(char* test_name);
int tester_utest_ipc_write8(char* test_name);
int tester_utest_ipc_write9(char* test_name);
int tester_utest_ipc_write10(char* test_name);


// Macro: checks that memory is "clean" before a test
/*#define TESTER_UTEST_IPC_ASSERT_CLEANUP()\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max, "_ipc_allocator not empty at the startup"));
        TESTER_UTEST_RESOURCE_ASSERT_CLEANUP();\
    }while(0)
*/
