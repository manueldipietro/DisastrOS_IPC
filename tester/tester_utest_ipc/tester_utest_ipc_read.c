#include "tester_ipc.h"
#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS_resource.h"
#include "disastrOS_ipc.h"

#include <stdio.h>

// Test 1: read a not empty buffer (with size >= count) (should return count)
int tester_utest_ipc_read1(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=10, size_max=15;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Manually set size for test environment
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    ipc->size = 15;
    // 3. Read from buffer
    return_value = disastrOS_read(file_descriptor, buffer, 10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error during disastrOS_read"));
    // 4. Check for attribute of ipc after read
    TESTER_UTEST_CHECK(tester_utest_assert_int(size_max-count, ipc->size, "after disastOS_read mismatch on ipc->size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(size_max, ipc->size_max, "after disastOS_read mismatch on ipc->size_max attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "after disastOS_read unexpected waiting process in ipc waiting_list_read"));
    // 5. Test ok, return 1
    return 1;
}

// Test 2: read an empty buffer and test reader block
void tester_utest_ipc_read2_aux(){disastrOS_exit(tester_utest_ipc_utils_reader(7, 10, 0, 0, 0));}
int tester_utest_ipc_read2(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, size_max=15, reader_pid;
    Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    // 2. Spawn sleeper and reader process, and wait for read execution
    disastrOS_spawn(tester_utest_ipc_utils_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read2_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that reader block correctly
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 1, reader_pid, "error after disastOS_read");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "error after disastOS_read mismatch on ipc->size attribute"));

    // 4. Test ok, return 1
    return 1;
}

// Test 3: read an empty buffer, after write on buffer and reader should unlock
void tester_utest_ipc_read3_aux(){disastrOS_exit(tester_utest_ipc_utils_reader(7, 10, 10, 1, 0));}
int tester_utest_ipc_read3(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=10, size_max=15, reader_pid;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Spawn sleeper and reader process, and wait for read execution
    disastrOS_spawn(tester_utest_ipc_utils_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read3_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that reader block correctly
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 1, reader_pid, "error after disastrOS_read");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastOS_read mismatch on ipc->size attribute"));

    // 4. Call write on the buffer for unlock reader and wait for read to execute
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error on ipc disastrOS_write"));
    disastrOS_sleep(1);

    // 5. Check that reader unblock and check ipc size
    TESTER_UTEST_IPC_ASSERT_READERS_UNLOCKANDEXITANDWAIT(ipc, 0, reader_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastOS_write mismatch on ipc->size attribute"));

    // 6. Test ok, return 1
    return 1;
}

// Test 4: read (with two reader) an empty buffer, after write on buffer (with enough byte) readers should unlock
void tester_utest_ipc_read4_aux(){disastrOS_exit(tester_utest_ipc_utils_reader(7, 7, 7, 1, 0));}
int tester_utest_ipc_read4(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=15, size_max=15, reader1_pid, reader2_pid;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Spawn sleeper and reader process, and wait for read execution
    disastrOS_spawn(tester_utest_ipc_utils_sleeper, 0);
    reader1_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read4_aux, 0);
    reader2_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read4_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that reader block correctly
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 2, reader1_pid, "error after disastrOS_read");
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 2, reader2_pid, "error after disastrOS_read");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastrOS_read mismatch on ipc->size attribute after read"));
    // 4. Call write on the buffer for unlock reader and wait for read to execute
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error during disastrOS_write"));
    disastrOS_sleep(3);
    // 5. Check that reader unblock and check ipc size
    TESTER_UTEST_IPC_ASSERT_READERS_UNLOCKANDEXITANDWAIT(ipc, 0, reader1_pid, "error on first reader unblock");
    TESTER_UTEST_IPC_ASSERT_READERS_UNLOCKANDEXITANDWAIT(ipc, 0, reader2_pid, "error on second reader unblock");
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, ipc->size, "after reader unblock mismatch on ipc->size attribute"));
    // 6. Test ok, return 1
    return 1;
}

// Test 5: read an empty buffer, then write in buffer (write_count > read_count), readers should unlock and return write_count (partially read)
void tester_utest_ipc_read5_aux(){disastrOS_exit(tester_utest_ipc_utils_reader(7, 10, 5, 1, 0));}
int tester_utest_ipc_read5(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=5, size_max=15, reader_pid;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during Ipc_open"));
    file_descriptor = return_value;
    // 2. Spawn sleeper and reader process, and wait for read execution
    disastrOS_spawn(tester_utest_ipc_utils_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read5_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that reader block correctly
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 1, reader_pid, "error after disastrOS_read");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastrOS_read mismatch on ipc->size attribute"));
    // 4. Call write on the buffer for unlock reader and wait for read to execute
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error during disastrOS_write"));
    disastrOS_sleep(1);
    // 5. Check that reader unblock and check ipc size
    TESTER_UTEST_IPC_ASSERT_READERS_UNLOCKANDEXITANDWAIT(ipc, 0, reader_pid, "error on read unblock");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after reader unblock mismatch on ipc->size attribute"));
    // 6. Test ok, return 1
    return 1;
}

// Test 6: write on a full buffer, then read and free buffer, writers will unblock and write
void tester_utest_ipc_read6_aux(){disastrOS_exit(tester_utest_ipc_utils_writer(7, 10, 10, 1, 0));}
int tester_utest_ipc_read6(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=10, size_max=15, writer_pid;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Manually set size for test environment
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    ipc->size = 15;
    // 3. Spawn sleeper and writer process, and wait for read execution
    disastrOS_spawn(tester_utest_ipc_utils_sleeper, 0);
    writer_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read6_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that writer block correctly
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size, "after disastrOS_write mismatch on ipc size attribute"));
    // 4. Call read on the buffer for unlock writer and wait for write to execute
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error on disastrOS_read"));
    disastrOS_sleep(1);
    // 5. Check that writer unblock and check ipc size
    TESTER_UTEST_IPC_ASSERT_WRITERS_UNLOCKANDEXITANDWAIT(ipc, 0, writer_pid, "error during unblock writer");
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size, "after unblock writer mismatch on ipc size attribute"));
    // 6. Test ok, return 1
    return 1;
}

// Test 7: read a partially-full buffer (with count > size_max), should read all available data
int tester_utest_ipc_read7(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=100, size_max=15;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Manually set size for test environment
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    ipc->size = 7;
    // 3. Read from buffer
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(7, return_value, "error during disastrOS_read"));
    // 4. Check for attribute of ipc after read
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastrOS_read mismatch on ipc size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "unexpected waiting process in ipc waiting_list_read"));
    // 5. Test ok, return 1
    return 1;
}

// Test 8: read with DSOS_O_NONBLOCK an empty buffer (should return EAGAIN immediately)
int tester_utest_ipc_read8(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=10, size_max=15;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc (with DSOS_O_NONBLOCK)
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Call write and check 
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EAGAIN, return_value, "error on read"));
    // 4. Check for attribute of ipc after read
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastrOS_Read mismatch on ipc->size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "after read unexpected waiting process in ipc waiting_list_read"));
    // 3. Test ok, return 1
    return 1;
}

// Test 9: read with DSOS_O_NONBLOCK with a non empty buffer
int tester_utest_ipc_read9(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=7, size_max=15;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc (with DSOS_O_NONBLOCK)
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Manually set size for test environment
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    ipc->size = 5;
    // 3. Call write and check 
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(5, return_value, "error on disastrOS_read"));
    // 4. Check for attribute of ipc after read
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastrOS_read mismatch on ipc->size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "after disastrOS_read unexpected waiting process in ipc waiting_list_read"));
    // 5. Test ok, return 1
    return 1;
}

// Test 10: read with count = 0 on a IPC with an other reader waiting. Read should return 0 immediately (without waiting) 
void tester_utest_ipc_read10_aux(){disastrOS_exit(tester_utest_ipc_utils_reader(7, 10, 0, 0, 0));}
int tester_utest_ipc_read10(char* test_name){
    // 0. Initialize
    int return_value, resource_id=7, file_descriptor, count=0, size_max=15, reader_pid;
    char buffer[1]; Ipc* ipc;
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mks"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Spawn sleeper and reader process, and wait for read execution
    disastrOS_spawn(tester_utest_ipc_utils_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_read2_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that reader block correctly
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 1, reader_pid, "error after disastrOS_read");
    // 4. Call reader with count = 0, should return immediately without waiting and without side-effect
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(0, return_value, "error on disastrOS_read with count = 0"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "after disastrOS_read with count = 0 mismatching on ipc->size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 1, "after disastrOS_read with count = 0 unexpected waiting process in ipc waiting_list_read"));
    // 5. Test ok, return 1
    return 1;
}

