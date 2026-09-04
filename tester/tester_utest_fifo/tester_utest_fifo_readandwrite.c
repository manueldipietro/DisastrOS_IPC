#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS_fifo.h"

// Test 1: Open in read mode (spawn a writer for allow open), try to read without reader and without data (should return EOF)
void tester_utest_fifo_read1_aux(){disastrOS_exit(tester_utest_fifo_utils_writer_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_read1(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, pid_writer_open, resource_id=10, count = 10;
    Fifo* fifo; char* buffer[10];
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create a fifo and retrieve from resource list
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkfifo"));
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));
    // 3. Spawn sleeper and writer opener process and sleep while opener running
    disastrOS_spawn(tester_aux_sleeper, 0);
    pid_writer_open = last_pid;
    disastrOS_spawn(tester_utest_fifo_read1_aux, 0);
    disastrOS_sleep(1);
    // 4. Open Fifo with DSOS_O_RDONLY and DSOS_O_NONBLOCK
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open"));
    file_descriptor = return_value;
    disastrOS_sleep(1);
    // 5. Wait for Writer opener and after check number of writers
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(pid_writer_open, 1, 0, 0, 0, "After disastrOS_write");
    disastrOS_sleep(1);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->writers_number, "error on writers number befor disastrOS_read"));
    // 6. Read from fifo, should return EOF (0)
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(0, return_value, "error, disastrOS_read doesn't return EOF"));
    // 7. Test ok, return 1
    return 1;
}

// Test 2: Spawn 1 reader (need for write mode open) and open in write_mode, try to write, should return DSOS_EPIPE
void tester_utest_fifo_write1_aux(){disastrOS_exit(tester_utest_fifo_utils_reader_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_write1(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id=10, count = 10;
    Fifo* fifo; char* buffer[10];
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create a fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Retrive fifo from resources list
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrive fifo from resources list"));
    // 3. Spawn sleeper and reader opener process and sleep while opener running
    disastrOS_spawn(tester_aux_sleeper, 0);
    disastrOS_spawn(tester_utest_fifo_write1_aux, 0);
    disastrOS_sleep(1);
    // 4. Open fifo in writer mode and wait for the reader
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open"));
    file_descriptor = return_value;
    disastrOS_sleep(1);
    // 5. Write on the fifo, should return DSOS_EPIPE
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EPIPE, return_value, "error, disastrOS_write doesn't return DSOS_EPIPE"));
    // 6. Test ok, return 1
    return 1;
}