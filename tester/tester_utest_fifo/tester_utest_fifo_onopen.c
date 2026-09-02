#include "tester_fifo.h"
#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_fifo.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS_pcb.h"

#include <stdio.h>

// Test 1: Try to open Fifo with DSOS_O_RDWR flags, should return DSOS_EINVAL
int tester_utest_fifo_onopen1(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create a fifo
    resource_id = 10;
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Open fifo with flag DSOS_O_RDWR (should return DSOS_EINVAL)
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error disastrOS open can't recongize DSOS_O_RDWR as invalid flag"));
    // 3. Check that open roll back correctly
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_ROLLBACK(resource_id, 1, 0, 0, 0, "After open");
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to open Fifo with DSOS_O_RDONLY without writer, should block open and put it in waiting_list_open_reader
void tester_utest_fifo_onopen2_aux(){disastrOS_exit(tester_utest_fifo_utils_reader_open(10, 0, 0, DSOS_SUCCESS));}
int tester_utest_fifo_onopen2(char* test_name){
    // 0. Initialization
    int return_value, resource_id=10;
    int pid_reader_open;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    
    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));

    // 3. Spawn sleeper and reader opener process
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    pid_reader_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_onopen2_aux, 0);
    
    // 4. Sleep while opener running
    disastrOS_sleep(1);
    
    // 5. Check if open effectively wait
    TESTER_UTEST_FIFO_ASSERT_OPEN_WAITING(pid_reader_open, DSOS_O_RDONLY, 0, 0, 1, 0, "reader open wait");

    // 6. Test ok, return 1
    return 1;
}

// Test 3: Try to open Fifo with DSOS_O_RDONLY without writer, then open as a writer and it should unlock and return file descriptor
void tester_utest_fifo_onopen3_aux(){disastrOS_exit(tester_utest_fifo_utils_reader_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_onopen3(char* test_name){
    // 0. Initialization
    int return_value, resource_id=10;
    int pid_reader_open;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    
    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));

    // 3. Spawn sleeper and reader opener process
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    pid_reader_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_onopen3_aux, 0);
    
    // 4. Sleep while opener running
    disastrOS_sleep(1);
    
    // 5. Check if open effectively wait
    TESTER_UTEST_FIFO_ASSERT_OPEN_WAITING(pid_reader_open, DSOS_O_RDONLY, 0, 0, 1, 0, "reader open wait");

    // 6. Open the fifo as writer
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on writer opener"));
    
    // 7. Sleep while opener running
    disastrOS_sleep(1);

    // 8. Check that reader opener process unblock
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(pid_reader_open, 0, 1, 0, 0, "unlock");
    // 9. Test ok, return 1
    return 1;
}

// Test 4: Try to open Fifo with DSOS_O_WRONLY whiutout reader, should block open and put it in waiting_list_open_writer
void tester_utest_fifo_onopen4_aux(){disastrOS_exit(tester_utest_fifo_utils_writer_open(10, 0, 0, DSOS_SUCCESS));}
int tester_utest_fifo_onopen4(char* test_name){
    // 0. Initialization
    int return_value, resource_id=10;
    int pid_writer_open;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));

    // 3. Spawn sleeper and writer opener process
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    pid_writer_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_onopen4_aux, 0);
    
    // 4. Sleep while opener running
    disastrOS_sleep(1);

    // 5. Check if open effectively wait
    TESTER_UTEST_FIFO_ASSERT_OPEN_WAITING(pid_writer_open, DSOS_O_WRONLY, 0, 0, 0, 1, "writer open wait");

    // 6. Test ok, return 1
    return 1;
}

// Test 5: Try to open Fifo with DSOS_O_WRONLY without reader, then spawn a reader and it should unlock and return file descriptor
void tester_utest_fifo_onopen5_aux(){disastrOS_exit(tester_utest_fifo_utils_writer_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_onopen5(char* test_name){
    // 0. Initialization
    int return_value, resource_id=10;
    int pid_writer_open;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));

    // 3. Spawn sleeper and writer opener process
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    pid_writer_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_onopen5_aux, 0);

    // 4. Sleep while opener running
    disastrOS_sleep(1);

    // 5. Check if open effectively wait
    TESTER_UTEST_FIFO_ASSERT_OPEN_WAITING(pid_writer_open, DSOS_O_WRONLY, 0, 0, 0, 1, "writer open wait");

    // 6. Open the fifo as writer
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on reader opener"));
    
    // 7. Sleep while opener running
    disastrOS_sleep(1);

    // 8. Check that reader opener process unblock
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(pid_writer_open, 1, 0, 0, 0, "unlock");

    // 9. Test ok, return 1
    return 1;
}

// Test 6: Try to open Fifo with DSOS_O_RDONLY|DSOS_O_NONBLOCK, without writer, should return descriptor
int tester_utest_fifo_onopen6(char* test_name){
    // 0. Initialization
    int return_value, resource_id=10;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));
    
    // 3. Open Fifo with DSOS_O_RDONLY and DSOS_O_NONBLOCK
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open"));

    // 4. Check memory
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-1, "_fifo_allocator mismatch"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator mismatch"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator mismatch"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "resources_list not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(fifo->ipc.resource.descriptors_ptrs), 1, "descriptor_ptrs list not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "descriptors list not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, fifo->readers_number, "mismatching on readers_number"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->writers_number, "mismatching on readers_number"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_reader, 0, "mismatching on waiting list open reader"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_writer, 0, "mismatching on waiting list open writer"));

    // 5. Test ok, return 1
    return 1;
}

// Test 7: Try to open Fifo with DSOS_O_WRONLY|DSOS_O_NONBLOCK, without reader, should return DSOS_ENXIO
int tester_utest_fifo_onopen7(char* test_name){
    // 0. Initialization
    int return_value, resource_id=10;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));
    
    // 3. Open Fifo with DSOS_O_WRONLY and DSOS_O_NONBLOCK
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENXIO, return_value, "error on disastrOS_open"));

    // 4. Check memory
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-1, "_fifo_allocator mismatch"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator mismatch"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator mismatch"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "resources_list mismatch"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(fifo->ipc.resource.descriptors_ptrs), 0, "descriptor_ptrs list not empty"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "descriptors list not empty"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->readers_number, "mismatching on readers_number"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->writers_number, "mismatching on readers_number"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_reader, 0, "mismatching on waiting list open reader"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_writer, 0, "mismatching on waiting list open writer"));


    // 5. Test ok, return 1
    return 1;
}

// Test 8: Try to open Fifo with DSOS_O_RDONLY|DSOS_O_NONBLOCK, with a writer, should return descriptor immediately
void tester_utest_fifo_onopen8_aux(){disastrOS_exit(tester_utest_fifo_utils_writer_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_onopen8(char* test_name){
    // 0. Initialization
    int return_value, pid_writer_open, resource_id=10;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));
    // 3. Spawn sleeper and writer opener process and sleep while opener running
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    pid_writer_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_onopen8_aux, 0);
    disastrOS_sleep(2);

    // 5. Open Fifo with DSOS_O_RDONLY and DSOS_O_NONBLOCK
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open"));
    disastrOS_sleep(1);
    
    // 6. Check
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(pid_writer_open, 1, 0, 0, 0, "After write");

    // 7. Test ok, return 1
    return 1;
}

// Test 9: Try to open Fifo with DSOS_O_WRONLY|DSOS_O_NONBLOCK, with a reader, should return descriptor immediately
void tester_utest_fifo_onopen9_aux(){disastrOS_exit(tester_utest_fifo_utils_reader_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_onopen9(char* test_name){
    // 0. Initialization
    int return_value, pid_reader_open, resource_id=10;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));
    // 3. Spawn sleeper and reader opener process and sleep while opener running
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    pid_reader_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_onopen9_aux, 0);
    disastrOS_sleep(2);

    // 5. Open Fifo with DSOS_O_RDONLY and DSOS_O_NONBLOCK
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_NONBLOCK);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open"));
    disastrOS_sleep(1);
    
    // 6. Check
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(pid_reader_open, 0, 1, 0, 0, "After open writer");

    // 7. Test ok, return 1
    return 1;
}
