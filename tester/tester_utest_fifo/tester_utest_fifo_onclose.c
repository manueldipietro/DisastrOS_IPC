#include "tester.h"

#include "disastrOS_fifo.h"
#include "disastrOS.h"

#include <stdio.h>

void tester_utest_fifo_onclose_auxR(){
    if(!tester_utest_fifo_utils_reader_open(45, 1, 0, DSOS_SUCCESS))
        exit(0);
    while(!0){
        disastrOS_sleep(1);
    }
    disastrOS_exit(1);
}
void tester_utest_fifo_onclose_auxW(){
    if(!tester_utest_fifo_utils_writer_open(45, 1, 0, DSOS_SUCCESS))
        exit(0);
    while(!0) disastrOS_sleep(1);
    disastrOS_exit(1);
}

// Test 1: Spawn 2 reader and 2 writer, after open process in read mode and check close decrement counter
int tester_utest_fifo_onclose1(){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 45;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create fifo 
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkfifo"));
    // 2. Spawn sleepers, 2 reader and 2 writer
    disastrOS_spawn(tester_aux_sleeper, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxR, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxR, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxW, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxW, 0);
    disastrOS_sleep(2);
    // 3. Open fifo in write mode
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 4. Check reader and writer counter
    Fifo* fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrieve fifo from resources list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, fifo->readers_number, "mismatching on fifo->readers_number after disastrOS_spawn"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(3, fifo->writers_number, "mismatching on fifo->writers_number after disastrOS_spawn"));
    // 5. Close fifo and check for counter decrement
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, fifo->readers_number, "mismatching on fifo->readers_number after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, fifo->writers_number, "mismatching on fifo->writers_number after disastrOS_close"));
    // 6. Test ok, return 1
    return 1;
}

// Test 2: Spawn 2 reader and 2 writer, after open process in write and check close decrement counter
int tester_utest_fifo_onclose2(){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 45;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkfifo"));
    // 2. Spawn sleepers, 2 reader and 2 writer
    disastrOS_spawn(tester_aux_sleeper, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxR, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxR, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxW, 0);
    disastrOS_spawn(tester_utest_fifo_onclose_auxW, 0);
    disastrOS_sleep(1);
    // 3. Open fifo in read mode
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 4. Check reader and writer counter
    Fifo* fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrieve fifo from resources list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(3, fifo->readers_number, "mismatching on fifo->readers_number after disastrOS_spanw"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, fifo->writers_number, "mismatching on fifo->writers_number after disastrOS_spanw"));
    // 5. Close fifo and check for counter decrement
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, fifo->readers_number, "mismatching on fifo->readers_number after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, fifo->writers_number, "mismatching on fifo->writers_number after disastrOS_close"));
    // 6. Test ok, return 1
    return 1;
}

// Test 3: Open in write mode on init and spawn a reader (will lock) after write close will return EOF
void tester_utest_fifo_onclose3_aux(){
    char buffer[15];
    disastrOS_exit(tester_utest_fifo_utils_reader(33, 10, 0, buffer, 1, 0));
}
int tester_utest_fifo_onclose3(){
    // 0. Initilize
    int return_value, file_descriptor, resource_id = 33, reader_pid;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkfifo"));
    // 2. Spawn sleepers and reader
    disastrOS_spawn(tester_aux_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_fifo_onclose3_aux, 0);
    // 3. Open in write mode and wait for child execution
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 4. Close and wait for readers
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    // 5. Wait for reader
    int ret_pid = disastrOS_wait(reader_pid, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, ret_pid, "error during disastrOS_wait"));
    if(!return_value) exit(0);
    // 6. Test ok, return 1
    return 1;
}

// Test 4: Open in read mode on init and spawn a writer (will lock) after read close will return EPIPE
void tester_utest_fifo_onclose4_aux(){
    char buffer[15];
    disastrOS_exit(tester_utest_fifo_utils_writer(33, 10, DSOS_EPIPE, buffer, 1, 0));
}
int tester_utest_fifo_onclose4(){
    // 0. Initilize
    int return_value, file_descriptor, resource_id = 33, reader_pid;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create fifo and set size for block writer
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkfifo"));
    Fifo* fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    fifo->ipc.size = 10;
    // 2. Spawn sleepers and reader
    disastrOS_spawn(tester_aux_sleeper, 0);
    reader_pid = last_pid;
    disastrOS_spawn(tester_utest_fifo_onclose4_aux, 0);
    // 3. Open in read mode and wait for child execution
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 4. Close and wait for readers
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    // 5. Wait for reader
    int ret_pid = disastrOS_wait(reader_pid, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, ret_pid, "error during disastrOS_wait"));
    if(!return_value) exit(0);
    // 6. Test ok, return 1
    return 1;
}
