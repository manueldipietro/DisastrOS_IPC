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
int tester_resource_mk1(char* test_name);
int tester_resource_mk2(char* test_name);
int tester_resource_mk3(char* test_name);
int tester_resource_mk4(char* test_name);
int tester_resource_open1(char* test_name);
int tester_resource_open2(char* test_name);
int tester_resource_open3(char* test_name);
int tester_resource_open4(char* test_name);
int tester_resource_open5(char* test_name);
int tester_resource_open6(char* test_name);
int tester_resource_open7(char* test_name);
int tester_resource_open8(char* test_name);
int tester_resource_open9(char* test_name);
int tester_resource_open10(char* test_name);
int tester_resource_open11(char* test_name);
int tester_resource_open12(char* test_name);
int tester_resource_open13(char* test_name);
int tester_resource_unlink1(char* test_name);
int tester_resource_unlink2(char* test_name);
int tester_resource_unlink3(char* test_name);
int tester_resource_unlink4(char* test_name);
int tester_resource_unlink5(char* test_name);
int tester_resource_unlink6(char* test_name);
int tester_resource_close1(char* test_name);
int tester_resource_close2(char* test_name);
int tester_resource_close3(char* test_name);
int tester_resource_close4(char* test_name);
int tester_resource_close5(char* test_name);
int tester_resource_close6(char* test_name);
int tester_resource_close7(char* test_name);
int tester_resource_write1(char* test_name);
int tester_resource_write2(char* test_name);
int tester_resource_write3(char* test_name);
int tester_resource_write4(char* test_name);
int tester_resource_write5(char* test_name);
int tester_resource_read1(char* test_name);
int tester_resource_read2(char* test_name);
int tester_resource_read3(char* test_name);
int tester_resource_read4(char* test_name);
int tester_resource_read5(char* test_name);

// Ipc utest
int tester_utest_ipc_mk1(char* test_name);
int tester_utest_ipc_mk2(char* test_name);
int tester_utest_ipc_mk3(char* test_name);
int tester_utest_ipc_mk4(char* test_name);
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

// Spawn_withfd utests
int tester_utest_spawnfd1(char* test_name);

// Circular buffer utest
int tester_utest_circular_buffer_1(char* test_name);
int tester_utest_circular_buffer_2(char* test_name);
int tester_utest_circular_buffer_3(char* test_name);
int tester_utest_circular_buffer_4(char* test_name);
int tester_utest_circular_buffer_5(char* test_name);
int tester_utest_circular_buffer_6(char* test_name);
int tester_utest_circular_buffer_7(char* test_name);
int tester_utest_circular_buffer_8(char* test_name);

// Fifo utest
int tester_utest_fifo_mk1(char* test_name);
int tester_utest_fifo_mk2(char* test_name);
int tester_utest_fifo_mk3(char* test_name);
int tester_utest_fifo_mk4(char* test_name);
int tester_utest_fifo_onopen1(char* test_name);
int tester_utest_fifo_onopen2(char* test_name);
int tester_utest_fifo_onopen3(char* test_name);
int tester_utest_fifo_onopen4(char* test_name);
int tester_utest_fifo_onopen5(char* test_name);
int tester_utest_fifo_onopen6(char* test_name);
int tester_utest_fifo_onopen7(char* test_name);
int tester_utest_fifo_onopen8(char* test_name);
int tester_utest_fifo_onopen9(char* test_name);
int tester_utest_fifo_read1(char* test_name);
int tester_utest_fifo_write1(char* test_name);
