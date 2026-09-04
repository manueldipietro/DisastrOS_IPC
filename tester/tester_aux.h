#pragma once

#define TESTER_ANSI_RESET           "\033[0m"
#define TESTER_ANSI_BOLD            "\033[1m"
#define TESTER_ANSI_BRIGHT_RED      "\033[91m"
#define TESTER_INIT_TAG             "\033[1m\033[93mINIT\033[0m| "
#define TESTER_READER_TAG           "\033[1m\033[94mREADER\033[0m| "
#define TESTER_WRITER_TAG           "\033[1m\033[95mWRITER\033[0m| "
#define TESTER_READER_TAG_WID       "\033[1m\033[94mREADER %d\033[0m| "
#define TESTER_WRITER_TAG_WID       "\033[1m\033[95mWRITER %d\033[0m| "
#define TESTER_CHILD_TAG_WID        "\033[1m\033[94mCHILD %d\033[0m| "

// Aux function
void tester_aux_sleeper();
int tester_utest_ipc_utils_writer(int resource_id, int count, int expected_write_count, int should_exit, int non_blocking);
int tester_utest_ipc_utils_reader(int resource_id, int count, int expected_read_count, int should_exit, int non_blocking);
int tester_utest_fifo_utils_reader_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval);
int tester_utest_fifo_utils_writer_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval);
int tester_utest_fifo_utils_reader(int resource_id, int count, int expected_read_count, char* buffer, int should_exit, int non_blocking);
int tester_utest_fifo_utils_writer(int resource_id, int count, int expected_read_count, char* buffer, int should_exit, int non_blocking);


// Integration tests
void test_itest_resource1_init();
void test_itest_fifo1_init();
void test_itest_fifo2_init();
void test_itest_fifo3_init();
void test_itest_fifo4_init();
void test_itest_pipe1_init();
void test_itest_pipe2_init();
void test_itest_pipe3_init();
void test_itest_pipe4_init();

// Utest executors
int tester_utest_resources();
int tester_utest_ipc();
int tester_utest_spawnfd();
int tester_utest_circular_buffer();
int tester_utest_fifo();

// Resource utest
int tester_resource_mk1();
int tester_resource_mk2();
int tester_resource_mk3();
int tester_resource_mk4();
int tester_resource_open1();
int tester_resource_open2();
int tester_resource_open3();
int tester_resource_open4();
int tester_resource_open5();
int tester_resource_open6();
int tester_resource_open7();
int tester_resource_open8();
int tester_resource_open9();
int tester_resource_open10();
int tester_resource_open11();
int tester_resource_open12();
int tester_resource_open13();
int tester_resource_unlink1();
int tester_resource_unlink2();
int tester_resource_unlink3();
int tester_resource_unlink4();
int tester_resource_unlink5();
int tester_resource_unlink6();
int tester_resource_close1();
int tester_resource_close2();
int tester_resource_close3();
int tester_resource_close4();
int tester_resource_close5();
int tester_resource_close6();
int tester_resource_close7();
int tester_resource_write1();
int tester_resource_write2();
int tester_resource_write3();
int tester_resource_write4();
int tester_resource_write5();
int tester_resource_read1();
int tester_resource_read2();
int tester_resource_read3();
int tester_resource_read4();
int tester_resource_read5();

// Ipc utest
int tester_utest_ipc_mk1();
int tester_utest_ipc_mk2();
int tester_utest_ipc_mk3();
int tester_utest_ipc_mk4();
int tester_utest_ipc_read1();
int tester_utest_ipc_read2();
int tester_utest_ipc_read3();
int tester_utest_ipc_read4();
int tester_utest_ipc_read5();
int tester_utest_ipc_read6();
int tester_utest_ipc_read7();
int tester_utest_ipc_read8();
int tester_utest_ipc_read9();
int tester_utest_ipc_read10();
int tester_utest_ipc_write1();
int tester_utest_ipc_write2();
int tester_utest_ipc_write3();
int tester_utest_ipc_write4();
int tester_utest_ipc_write5();
int tester_utest_ipc_write6();
int tester_utest_ipc_write7();
int tester_utest_ipc_write8();
int tester_utest_ipc_write9();
int tester_utest_ipc_write10();

// Spawn_withfd utests
int tester_utest_spawnfd1();

// Circular buffer utest
int tester_utest_circular_buffer_1();
int tester_utest_circular_buffer_2();
int tester_utest_circular_buffer_3();
int tester_utest_circular_buffer_4();
int tester_utest_circular_buffer_5();
int tester_utest_circular_buffer_6();
int tester_utest_circular_buffer_7();
int tester_utest_circular_buffer_8();

// Fifo utest
int tester_utest_fifo_mk1();
int tester_utest_fifo_mk2();
int tester_utest_fifo_mk3();
int tester_utest_fifo_mk4();
int tester_utest_pipe_mk1();
int tester_utest_pipe_mk2();
int tester_utest_pipe_mk3();
int tester_utest_pipe_mk4();
int tester_utest_fifo_onopen1();
int tester_utest_fifo_onopen2();
int tester_utest_fifo_onopen3();
int tester_utest_fifo_onopen4();
int tester_utest_fifo_onopen5();
int tester_utest_fifo_onopen6();
int tester_utest_fifo_onopen7();
int tester_utest_fifo_onopen8();
int tester_utest_fifo_onopen9();
int tester_utest_fifo_onclose1();
int tester_utest_fifo_onclose2();
int tester_utest_fifo_onclose3();
int tester_utest_fifo_onclose4();
int tester_utest_fifo_read1();
int tester_utest_fifo_write1();
