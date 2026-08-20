#include "tester_ipc.h"
#include "tester.h"

#include "disastrOS_resource.h"
#include "disastrOS_ipc.h"
#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS_pcb.h"

#include <stdio.h>
#include <assert.h>

void tester_utest_ipc_sleeper(void* args){
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

// Test 1: Try to call write with empty buffer with count <= size_max, should write with success without blocking process
int tester_utest_ipc_write1(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    Ipc* ipc; char buffer[15];
    // ASSERT INIZIALE

    // 1. Create and open the resource
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(10, return_value, "error on ipc write"));
    
    // 3. Check for attributes of IPC after write
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size_max, "mismatch on size_max attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "mismatch on size attribute"));

    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to call write with insufficient free bytes, the process should blocking
int tester_utest_ipc_write2_writer(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 10);

    // 3. This will unlocking never, but if this happens this rise an assert
    assert(0 && "This will unlocking never");   
    return 0;
}

void tester_utest_ipc_write2_writer_aux(){
    disastrOS_exit(tester_utest_ipc_write2_writer());
}

int tester_utest_ipc_write2(char* test_name){
    // 0. Initialize
    int return_value, resource_id, size_max;
    int pid_writer;
    Ipc* ipc;
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource and open
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 6;

    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_writer = last_pid;
    disastrOS_spawn(tester_utest_ipc_write2_writer_aux, 0);

    // 4. Sleep waiting write being executed
    disastrOS_sleep(3);

    // 5. Check that the process is effectively waiting
    PCB* writer;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "mismatching on ipc waiting_list_write"));
    writer = PCB_byPID(&(ipc->waiting_list_write), pid_writer);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer, "writer not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer->status, "writer not in waiting status"));

    // 6. Check that the ipc size wasn't modified
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "unexpected change on ipc->size"));

    // 7. Test ok, return 1
    return 1;
}


// Test 3: Try to call write with insufficient free bytes, the process should blocking. After call read and check that write will unlocked
int tester_utest_ipc_write3_writer(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 10);

    // 3. This will unlock after the read on the test init
    return 1;
}

void tester_utest_ipc_write3_writer_aux(){
    disastrOS_exit(tester_utest_ipc_write3_writer());
}

int tester_utest_ipc_write3(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    int return_pid, pid_writer;
    Ipc* ipc; char buffer[15];
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 6;

    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_writer = last_pid;
    disastrOS_spawn(tester_utest_ipc_write3_writer_aux, 0);

    // 4. Sleep waiting write being executed
    disastrOS_sleep(3);

    // 5. Check that the process is effectively waiting
    PCB* writer;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "mismatching on ipc waiting_list_write"));
    writer = PCB_byPID(&(ipc->waiting_list_write), pid_writer);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer, "writer not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer->status, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "unexpected change on ipc->size"));

    // 6. Open resource and read blocking reader
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, 6);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(6, return_value, "error on read"));

    // 7. Check that writers unlock correctly
    disastrOS_sleep(3);
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "mismatching on ipc waiting_list_write after read"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Zombie, writer->status, "writer not in waiting status"));
    return_pid = disastrOS_wait(pid_writer, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(return_pid, pid_writer, "error during wait"));
    if(return_value == 0) return 0; // Bypass for errors
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "incorrect ipc->size value after write"));

    // 8. Test ok, return 1
    return 1;
}

// Test 4: Try to call write with insufficient free bytes, the process should blocking. After call read, and check that write will unlock and unlock another write
int tester_utest_ipc_write4_writer(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 7);

    // 3. This will unlock after the read on the test init
    return 1;
}

void tester_utest_ipc_write4_writer_aux(){
    disastrOS_exit(tester_utest_ipc_write4_writer());
}

int tester_utest_ipc_write4(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    int return_pid, pid_writer_1, pid_writer_2;
    Ipc* ipc; char buffer[15];
    PCB* writer_1; PCB* writer_2;
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 10;

    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_writer_1 = last_pid;
    disastrOS_spawn(tester_utest_ipc_write4_writer_aux, 0);
    pid_writer_2 = last_pid;
    disastrOS_spawn(tester_utest_ipc_write4_writer_aux, 0);

    // 4. Sleep waiting write being executed
    disastrOS_sleep(3);

    // 5. Check that the writer 1 and writer_2 is effectively waiting
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 2, "mismatching on ipc waiting_list_write"));
    writer_1 = PCB_byPID(&(ipc->waiting_list_write), pid_writer_1);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer_1, "writer_1 not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer_1, writer_1->pid, "writer_1 not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer_1->status, "writer_1 not in waiting status"));
    writer_2 = PCB_byPID(&(ipc->waiting_list_write), pid_writer_2);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer_2, "writer_2 not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer_2, writer_2->pid, "writer_2 not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer_2->status, "writer_2 not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "unexpected change on ipc->size"));

    // 6. Open resource and read unblocking reader
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, 10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(10, return_value, "error on read"));

    // 7. Check that writers unlock correctly
    disastrOS_sleep(6);
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "mismatching on ipc waiting_list_write after read"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer_1, writer_1->pid, "writer_1 in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Zombie, writer_1->status, "writer_1 in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer_2, writer_2->pid, "writer_2 in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Zombie, writer_2->status, "writer_2 in waiting status"));

    return_pid = disastrOS_wait(pid_writer_1, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(return_pid, pid_writer_1, "error during wait_1"));
    if(return_value == 0) return 0; // Bypass for errors
    
    return_pid = disastrOS_wait(pid_writer_2, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(return_pid, pid_writer_2, "error during wait_2"));
    if(return_value == 0) return 0; // Bypass for errors
    TESTER_UTEST_CHECK(tester_utest_assert_int(14, ipc->size, "incorrect ipc->size value after write"));

    // 8. Test ok, return 1
    return 1;
}

// Test 5: Try to call write with insufficient free bytes, the process should blocking. After call read insufficient byte and check that write will remain blocked
int tester_utest_ipc_write5_writer(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 10);

    // 3. This will unlocking never, but if this happens this rise an assert
    assert(0 && "This will unlocking never");   
    
    return 1;
}

void tester_utest_ipc_write5_writer_aux(){
    disastrOS_exit(tester_utest_ipc_write5_writer());
}

int tester_utest_ipc_write5(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    int pid_writer;
    Ipc* ipc; char buffer[15];
    PCB* writer;
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from resources_list"));
    ipc->size = 10;

    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_writer = last_pid;
    disastrOS_spawn(tester_utest_ipc_write5_writer_aux, 0);

    // 4. Sleep waiting write being executed
    disastrOS_sleep(3);

    // 5. Check that the process is effectively waiting
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "mismatching on ipc waiting_list_write"));
    writer = PCB_byPID(&(ipc->waiting_list_write), pid_writer);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer, "writer not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer->status, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "unexpected change on ipc->size"));

    // 6. Open resource and read
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, 2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error on read"));
    disastrOS_sleep(3);

    // 7. Check that writers already blocked
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "mismatching on ipc waiting_list_write"));
    writer = PCB_byPID(&(ipc->waiting_list_write), pid_writer);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer, "writer not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer->status, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(8, ipc->size, "unexpected change on ipc->size"));

    // 8. Test ok, return 1
    return 1;
}

// Test 6: Try to call reading on an empty buffer and check that write unlock the reader
int tester_utest_ipc_write6_reader(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_read(file_descriptor, buffer, 10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error on disastrOS_read"));

    return 1;
}

void tester_utest_ipc_write6_reader_aux(){
    disastrOS_exit(tester_utest_ipc_write6_reader());
}

int tester_utest_ipc_write6(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    int return_pid, pid_reader;
    Ipc* ipc; char buffer[15];
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource and retrieve IPC
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from resources_list"));

    // 3. Spawn readers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_reader = last_pid;
    disastrOS_spawn(tester_utest_ipc_write6_reader_aux, 0);

    // 4. Sleep waiting read being executed
    disastrOS_sleep(3);

    // 5. Check that read block
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 1, "mismatching on ipc waiting_list_read"));

    /* --§§Direi che non vale la pena controllare se la read è bloccante, ci penseranno i suoi test
    // 5. Check that the process is effectively waiting
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "mismatching on ipc waiting_list_write"));
    writer = PCB_byPID(&(ipc->waiting_list_write), pid_writer);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer, "writer not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer->status, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(10, ipc->size, "unexpected change on ipc->size"));
    */

    // 6. Open resource and write
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;
    return_value = disastrOS_write(file_descriptor, buffer, 2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error on write"));
    disastrOS_sleep(3);

    // 7. Check that reader unblocked
    return_pid = disastrOS_wait(pid_reader, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_reader, return_pid, "error during wait"));
    if(return_value == 0) return 0;

    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "mismatching on ipc waiting_list_read"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "unexpected change on ipc->size"));

    // 8. Test ok, return 1
    return 1;
}

// Test 7: Try to call write with count > size_max
int tester_utest_ipc_write7_writer(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 100);

    // 3. This will unlock after the read on the test init
    return 1;
}

void tester_utest_ipc_write7_writer_aux(){
    disastrOS_exit(tester_utest_ipc_write7_writer());
}

int tester_utest_ipc_write7(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    int return_pid, pid_writer;
    Ipc* ipc; char buffer[15];
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));

    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_writer = last_pid;
    disastrOS_spawn(tester_utest_ipc_write7_writer_aux, 0);

    // 4. Sleep waiting write being executed
    disastrOS_sleep(3);
    
    // 6. Open resource and read while readed != 100
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;
    
    int readed = 0;
    while(readed < 100){
        return_value = disastrOS_read(file_descriptor, buffer, 5);
        TESTER_UTEST_CHECK(tester_utest_assert_ecode(5, return_value, "error on read"));
        readed += return_value;
    }
    TESTER_UTEST_CHECK(tester_utest_assert_int(100, readed, "Error on total read"));

    return_pid = disastrOS_wait(pid_writer, &return_value);
    if(return_value == 0)return 0;

    //Qui si dovrebbe verificare anche che la write abbia cessato gli effetti?

    // Test ok, return 1
    return 1;
}





// Test 8: Try to call write, with DSOS_O_NONBLOCK on a full buffer (should return DSOS_EAGAIN)
int tester_utest_ipc_write8(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    Ipc* ipc; char buffer[15];
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource and open with DSOS_O_NONBLOCK
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 15;

    // 3. Call write non blocking (should return DSOS_EAGAIN)
    return_value = disastrOS_write(file_descriptor, buffer, 5);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EAGAIN, return_value, "error on disastrOS write"));

    // 4. Check for unexpected side effect
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "side effect on ipc waiting_list_write after write"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size, "side effect on ipc->size after write"));

    // 5. Test ok, return 1
    return 1;
}

// Test 9: Try to call write, with DSOS_O_NONBLOCK on a partially filled buffer
int tester_utest_ipc_write9(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    Ipc* ipc; char buffer[15];
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource and open with DSOS_O_NONBLOCK
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 13;

    // 3. Call write non blocking (should return 2)
    return_value = disastrOS_write(file_descriptor, buffer, 5);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(2, return_value, "error on disastrOS write"));

    // 4. Write should write only available bytes without blocking
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "side effect on ipc waiting_list_write after write"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(15, ipc->size, "mismatching ipc->size after write"));

    // 5. Test ok, return 1
    return 1;
}

// Test 10: Try to write on ipc with count = 0 and process already waiting
int tester_utest_ipc_write10_writer(){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    char buffer[15];
    
    // 1. Open resource
    resource_id = 10;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;

    // 2. Call write
    return_value = disastrOS_write(file_descriptor, buffer, 10);

    // 3. This will unlock after the read on the test init
    return 1;
}

void tester_utest_ipc_write10_writer_aux(){
    disastrOS_exit(tester_utest_ipc_write10_writer());
}

int tester_utest_ipc_write10(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, size_max;
    int pid_writer;
    Ipc* ipc; char buffer[15];
    setupSignals();
    // ASSERT INIZIALE PER PULIZIA; DA FARE

    // 1. Create resource
    resource_id = 10; size_max = 15;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on ipc creation"));

    // 2. Set manually ipc->size for force the process to block
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc from reosurces_list"));
    ipc->size = 6;

    // 3. Spawn writers and sleepers
    disastrOS_spawn(tester_utest_ipc_sleeper, 0);
    pid_writer = last_pid;
    disastrOS_spawn(tester_utest_ipc_write10_writer_aux, 0);

    // 4. Sleep waiting write being executed
    disastrOS_sleep(3);

    // 5. Check that the process is effectively waiting
    PCB* writer;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "mismatching on ipc waiting_list_write"));
    writer = PCB_byPID(&(ipc->waiting_list_write), pid_writer);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(writer, "writer not in the ipc waiting list"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(pid_writer, writer->pid, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, writer->status, "writer not in waiting status"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "unexpected change on ipc->size"));

    // 6. Open resource
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(0, return_value, "error on ipc open"));
    file_descriptor = return_value;
    
    // 7. Call write with count = 0
    return_value = disastrOS_write(file_descriptor, buffer, 0);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(0, return_value, "error on write with count = 0"));

    // 8. Check for unexpected side effect
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 1, "side effect on ipc waiting_list_write after write with count=0"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, ipc->size, "side effect on ipc->size after write with count=0"));

    // 9. Test ok, return 1
    return 1;
}
