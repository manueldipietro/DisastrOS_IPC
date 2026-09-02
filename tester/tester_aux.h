#pragma once

#define TESTER_ANSI_RESET           "\033[0m"
#define TESTER_ANSI_BOLD            "\033[1m"
#define TESTER_ANSI_BRIGHT_RED      "\033[91m"
#define TESTER_INIT_TAG             "\033[1m\033[93mINIT\033[0m| "
#define TESTER_READER_TAG           "\033[1m\033[94mREADER\033[0m| "
#define TESTER_WRITER_TAG           "\033[1m\033[95mWRITER\033[0m| "
#define TESTER_READER_TAG_WID       "\033[1m\033[94mREADER %d\033[0m| "
#define TESTER_WRITER_TAG_WID       "\033[1m\033[95mWRITER %d\033[0m| "
#define TESTER_CHILD_TAG_WID     "\033[1m\033[94mCHILD %d\033[0m| "

void tester_aux_sleeper();



// Resource utest

// Spawn_withfd utests

// Ipc utest

// SpawnWithfd

// Circular buffer utest
int tester_utest_circular_buffer();
int tester_utest_circular_buffer_1(char* test_name);
int tester_utest_circular_buffer_2(char* test_name);
int tester_utest_circular_buffer_3(char* test_name);
int tester_utest_circular_buffer_4(char* test_name);
int tester_utest_circular_buffer_5(char* test_name);
int tester_utest_circular_buffer_6(char* test_name);
int tester_utest_circular_buffer_7(char* test_name);
int tester_utest_circular_buffer_8(char* test_name);

// Fifo utest