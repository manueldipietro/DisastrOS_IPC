#include "tester.h"

#include "disastrOS_resource.h"
#include "disastrOS_ipc.h"
#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS_pcb.h"

#include <stdio.h>
#include <assert.h>

// Test 1: Try to call write with empty buffer with count <= size_max, should write with success without blocking process
int tester_utest_ipc_write1(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id=10, size_max=15;
    Ipc* ipc; char buffer[1];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on disastrOS_open"));
    file_descriptor = return_value;
    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(10, return_value, "error on disastrOS_write"));
    // 3. Check for attributes of IPC after write
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size_max, "after disastOS_write mismatch on ipc->size_max attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "after disastOS_write mismatch on ipc->size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "after disastOS_read unexpected waiting process in ipc->waiting_list_write"));
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to call write with insufficient free bytes, the process should blocking
void tester_utest_ipc_write2_aux(){
    disastrOS_exit(tester_utest_ipc_utils_writer(10, 10, 0, 0, 0));
}
int tester_utest_ipc_write2(char* test_name){
    // 0. Initialize
    int return_value, writer_pid, resource_id=10, size_max=15;
    Ipc* ipc;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create resource and open
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));
    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 6;
    // 3. Spawn writers and sleepers, and wait for read execution
    disastrOS_spawn(tester_aux_sleeper, 0);
    writer_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write2_aux, 0);
    disastrOS_sleep(1);
    // 4. Check that the process is effectively waiting
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "error after disastrOS_write mismatch on ipc->size attribute"));
    // 5. Test ok, return 1
    return 1;
}

// Test 3: Try to call write with insufficient free bytes, the process should blocking. After call read and check that write will unlocked
void tester_utest_ipc_write3_aux(){
    disastrOS_exit(tester_utest_ipc_utils_writer(10, 10, 10, 1, 0));
}
int tester_utest_ipc_write3(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15, writer_pid;
    Ipc* ipc; char buffer[15];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create resource and manualy set ipc->size to force write block
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "error during disastrOS_open"));
    ipc->size = 6;
    // 2. Spawn writers and sleepers, and wait for writers execution
    disastrOS_spawn(tester_aux_sleeper, 0);
    writer_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write3_aux, 0);
    disastrOS_sleep(1);
    // 4. Check that the process is effectively waiting
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "unexpected change on ipc->size"));
    // 5. Open resource and read unlocking writer
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, 6);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(6, return_value, "error on disastrOS_read"));
    disastrOS_sleep(1);
    // 6. Check that writers unlock correctly
    TESTER_UTEST_IPC_ASSERT_WRITERS_UNLOCKANDEXITANDWAIT(ipc, 0, writer_pid, "after disastrOS_read");
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "after disastrOS_read error mismatching on ipc->size value"));
    // 7. Test ok, return 1
    return 1;
}

// Test 4: Try to call write with insufficient free bytes, the process should blocking. After call read, and check that write will unlock and unlock another write
void tester_utest_ipc_write4_aux(){
    disastrOS_exit(tester_utest_ipc_utils_writer(10, 7, 7, 1, 0));
}
int tester_utest_ipc_write4(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15, writer1_pid, writer2_pid;
    Ipc* ipc; char buffer[15];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create resource and open IPC
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from list"));
    ipc->size = 10;
    // 3. Spawn writers and sleepers, and waiting for writes execution
    disastrOS_spawn(tester_aux_sleeper, 0);
    writer1_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write4_aux, 0);
    writer2_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write4_aux, 0);
    disastrOS_sleep(3);
    // 4. Check that the writer 1 and writer_2 is effectively waiting
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 2, writer1_pid, "error after disastrOS_write");
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 2, writer2_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "after disastrOS_write mismatch on ipc->size attribute after write"));
    // 5. Read unblocking writer
    return_value = disastrOS_read(file_descriptor, buffer, 10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(10, return_value, "error on read"));
    disastrOS_sleep(3);
    // 6. Check that writers unlock correctly
    TESTER_UTEST_IPC_ASSERT_WRITERS_UNLOCKANDEXITANDWAIT(ipc, 0, writer1_pid, "error on first writer unblock");
    TESTER_UTEST_IPC_ASSERT_WRITERS_UNLOCKANDEXITANDWAIT(ipc, 0, writer2_pid, "error on second writer unblock");
    TESTER_UTEST_CHECK(tester_utest_assert_int(14, ipc->size, "after reader unblock mismatch on ipc->size attribute"));
    // 7. Test ok, return 1
    return 1;
}

// Test 5: Try to call write with insufficient free bytes, the process should blocking. After call read insufficient byte and check that write will remain blocked
void tester_utest_ipc_write5_aux(){
    disastrOS_exit(tester_utest_ipc_utils_writer(10, 10, 0, 0, 0));
}
int tester_utest_ipc_write5(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15, writer_pid;
    Ipc* ipc; char buffer[15];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from resources_list"));
    ipc->size = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during Ipc_open"));
    file_descriptor = return_value;
    // 3. Spawn writers and sleepers, and wait for write execute
    disastrOS_spawn(tester_aux_sleeper, 0);
    writer_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write5_aux, 0);
    disastrOS_sleep(1);
    // 4. Check that the process is effectively waiting
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "after disastrOS_write mismatch on ipc->size attribute after write"));
    // 5. Call read on the buffer (with insufficient byte)
    return_value = disastrOS_read(file_descriptor, buffer, 2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error during disastrOS_read"));
    disastrOS_sleep(1);
    // 6. Check that writers already blocked
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_read");
    TESTER_UTEST_CHECK(tester_utest_assert_int(8, ipc->size, "after disastrOS_read mismatch on ipc->size attribute after write"));
    // 7. Test ok, return 1
    return 1;
}

// Test 6: Try to call reading on an empty buffer and check that write unlock the reader
void tester_utest_ipc_write6_aux(){
    disastrOS_exit(tester_utest_ipc_utils_reader(10, 10, 2, 1, 0));
}
int tester_utest_ipc_write6(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15, reader_pid;
    Ipc* ipc; char buffer[1];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create and open ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on Ipc_open"));
    file_descriptor = return_value;
    // 2. Spawn readers and sleeper and wait for write execution
    disastrOS_spawn(tester_aux_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write6_aux, 0);
    disastrOS_sleep(1);
    // 3. Check that read block correctly
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from resources_list"));
    TESTER_UTEST_IPC_ASSERT_READERS_WAITING(ipc, 1, reader_pid, "error after disastrOS_read");
    // 4. Open resource and write
    return_value = disastrOS_write(file_descriptor, buffer, 2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error during disastrOS_write"));
    disastrOS_sleep(1);
    // 5. Check that reader unblocked
    TESTER_UTEST_IPC_ASSERT_READERS_UNLOCKANDEXITANDWAIT(ipc, 0, reader_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "error unexpected value on ipc->size"));
    // 6. Test ok, return 1
    return 1;
}

// Test 7: Try to call write with count > size_max
void tester_utest_ipc_write7_aux(){
    disastrOS_exit(tester_utest_ipc_utils_writer(10, 100, 100, 1, 0));
}
int tester_utest_ipc_write7(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15, writer_pid;
    char buffer[1]; Ipc* ipc;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create and open Ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disatrOS_open"));
    // 2. Spawn writers and sleepers and wait for writer to be executed
    disastrOS_spawn(tester_aux_sleeper, 0);
    writer_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write7_aux, 0);
    disastrOS_sleep(1);
    // 3. Open resource and read while readed != 100
    file_descriptor = return_value;
    int readed = 0;
    while(readed < 100){
        return_value = disastrOS_read(file_descriptor, buffer, 5);
        TESTER_UTEST_CHECK(tester_utest_assert_ecode(5, return_value, "error during disastrOS_read"));
        readed += return_value;
    }
    TESTER_UTEST_CHECK(tester_utest_assert_int(100, readed, "error after disastrOS_read"));
    // 4. Wait writers
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from resources_list"));
    TESTER_UTEST_IPC_ASSERT_WRITERS_UNLOCKANDEXITANDWAIT(ipc, 0, writer_pid, "error after disastrOS_read");
    // 5. Test ok, return 1
    return 1;
}

// Test 8: Try to call write, with DSOS_O_NONBLOCK on a full buffer (should return DSOS_EAGAIN)
int tester_utest_ipc_write8(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15;
    Ipc* ipc; char buffer[15];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create resource and open with DSOS_O_NONBLOCK
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 15;
    // 3. Call write non blocking (should return DSOS_EAGAIN)
    return_value = disastrOS_write(file_descriptor, buffer, 5);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EAGAIN, return_value, "error during disastrOS_write"));
    // 4. Check for unexpected side effect
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "after disastrOS_write unexpected change on ipc waiting_list_write after write"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size, "after disastrOS_write unexpected side effect on ipc->size after write"));
    // 5. Test ok, return 1
    return 1;
}

// Test 9: Try to call write, with DSOS_O_NONBLOCK on a partially filled buffer
int tester_utest_ipc_write9(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15;
    Ipc* ipc; char buffer[15];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create resource and open with DSOS_O_NONBLOCK
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 13;
    // 3. Call write non blocking (should return 2)
    return_value = disastrOS_write(file_descriptor, buffer, 5);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error during disastrOS_write"));
    // 4. Write should write only available bytes without blocking
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "side effect on ipc waiting_list_write after write"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size, "mismatching on ipc->size after write"));
    // 5. Test ok, return 1
    return 1;
}

// Test 10: Try to write on ipc with count = 0 and process already waiting
void tester_utest_ipc_write10_aux(){
    disastrOS_exit(tester_utest_ipc_utils_writer(10, 10, 0, 0, 0));
}
int tester_utest_ipc_write10(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 10, size_max = 15, writer_pid;
    Ipc* ipc; char buffer[15];
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create resource
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during Ipc_mk"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 6;
    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_aux_sleeper, 0);
    writer_pid = last_pid;
    disastrOS_spawn(tester_utest_ipc_write10_aux, 0);
    disastrOS_sleep(1);
    // 4. Check that the process is effectively waiting
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_write");
    // 5. Call write with count = 0
    return_value = disastrOS_write(file_descriptor, buffer, 0);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(0, return_value, "error on write with count = 0"));
    // 6. Check for unexpected side effect
    TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(ipc, 1, writer_pid, "error after disastrOS_write");
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "mismatching on ipc->size after write with count=0"));
    // 7. Test ok, return 1
    return 1;
}
