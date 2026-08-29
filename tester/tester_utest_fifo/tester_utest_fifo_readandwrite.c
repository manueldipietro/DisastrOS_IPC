#include "tester_fifo.h"
#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include "disastrOS_fifo.h"

// Test 1: Open in read mode (spawn a writer for allow open) and try to read, should return EOF
void tester_utest_fifo_read1_aux(){disastrOS_exit(tester_utest_fifo_utils_writer_open(10, 1, 0, DSOS_SUCCESS));}
int tester_utest_fifo_read1(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, pid_writer_open, resource_id=10, count = 10;
    Fifo* fifo; char* buffer[10];
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
    disastrOS_spawn(tester_utest_fifo_read1_aux, 0);
    disastrOS_sleep(2);

    // 5. Open Fifo with DSOS_O_RDONLY and DSOS_O_NONBLOCK
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open"));
    file_descriptor = return_value;
    disastrOS_sleep(1);
    
    // 6. Wait for Writer opener and after check number of writers
    TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(pid_writer_open, 1, 0, 0, 0, "After write");
    disastrOS_sleep(1);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->writers_number, "error on writers number befor read"));

    // 7. Read from fifo, should return EOF (0)
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(0, return_value, "error, read doesn't return EOF"));

    // 9. Test ok, return 1
    return 1;
}

// Test 2: Spawn 1 reader (need for write mode open) and open in write_mode, try to write, should return DSOS_EPIPE


// Test 3: One writer write on buffer and one reader read from buffer
