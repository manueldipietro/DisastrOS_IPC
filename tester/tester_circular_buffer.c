#include "tester_circular_buffer.h"
#include "tester.h"

#include "circular_buffer.h"
#include "disastrOS_constants.h"
#include "disastrOS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tester_utest_circular_buffer(){
    int is_all_test_ok = 1;
    
    // Test 1: Write without Wrap-around: write on an empty buffer, with to_write < size_max
    // Test 2: Read without Wrap-around: read to_read byte < size bytes (number of bytes in the circular buffer)
    // Test 3: Write with Wrap-around: write on a buffer and check that Wrap-arround work properly
    // Test 4: Read with Wrap-around: read on a buffer and check that Wrap-arround work properly
    // Test 5: Write all empty buffer: Write size_max bytes with empty buffer (write_pos = 0)
    // Test 6: Read all full buffer: size_max bytes with full buffer (read_pos = 0)
    // Test 7: Write all empty buffer: Write size_max bytes with empty buffer (write_pos != 0)
    // Test 8: Read all full buffer: size_max bytes with full buffer (read_pos != 0)

    tester_utest_list utest_circular_buffer[] = {
        { .title = "Test circular_buffer 1: write without wrap-around", .utest_fn = tester_utest_circular_buffer_1},
        { .title = "Test circular_buffer 2: read without wrap-around", .utest_fn = tester_utest_circular_buffer_2},
        { .title = "Test circular_buffer 3: write with wrap-around", .utest_fn = tester_utest_circular_buffer_3},
        { .title = "Test circular_buffer 4: read with wrap-around", .utest_fn = tester_utest_circular_buffer_4},
        { .title = "Test circular_buffer 5: write full buffer (from start_pos)", .utest_fn = tester_utest_circular_buffer_5},
        { .title = "Test circular_buffer 6: read full buffer (from start_pos)", .utest_fn = tester_utest_circular_buffer_6},
        { .title = "Test circular_buffer 7: write full buffer (from middle pos)", .utest_fn = tester_utest_circular_buffer_7},
        { .title = "Test circular_buffer 8: read full buffer (from middle pos)", .utest_fn = tester_utest_circular_buffer_8},
    };

    printf("Executing utest for circular buffer:\n");
    is_all_test_ok *= tester_utest_executelist(utest_circular_buffer, sizeof(utest_circular_buffer)/sizeof(utest_circular_buffer[0]), "circular buffer") ? 1 : 0;

    return is_all_test_ok;
}

// Test 1: Write without Wrap-around: write on an empty buffer, with to_write < size_max
int tester_utest_circular_buffer_1(char* test_name){
    // 0. Initialization
    const char user_buffer[10] = "CIAOCIAOCI"; char system_buffer[10];
    int to_write = 8, size_max = 10, write_pos = 0;
    memset(system_buffer, ' ', 10); const char expected_buffer[10] = "CIAOCIAO  ";
    // 1. Write on buffer
    Circular_buffer_write(user_buffer, system_buffer, to_write, size_max, &write_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(8, write_pos, "mismatching on write_pos"));
    // 2. Check buffer
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(system_buffer, expected_buffer, 10), "buffer not as expected"));
    // 3. Test ok, return 1
    return 1;
}

// Test 2: Read without Wrap-around: read to_read byte < size bytes (number of bytes in the circular buffer)
int tester_utest_circular_buffer_2(char* test_name){
    // 0. Initialization
    const char system_buffer[10] = "CIAOCIAOCI"; char user_buffer[10];
    int to_read = 8, size_max = 10, read_pos = 0;
    memset(user_buffer, ' ', 10); const char expected_buffer[10] = "CIAOCIAO  ";
    // 1. Read on buffer
    Circular_buffer_read(system_buffer, user_buffer, to_read, size_max, &read_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(8, read_pos, "mismatching on read_pos"));
    // 2. Check buffer
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(user_buffer, expected_buffer, 10), "buffer not as expected"));
    return 1;
}

// Test 3: Write with Wrap-around: write on a buffer and check that Wrap-arround work properly
int tester_utest_circular_buffer_3(char* test_name){
    // 0. Initialization
    const char user_buffer[10] = "CIAOCIAOCI"; char system_buffer[10];
    int to_write = 6, size_max = 10, write_pos = 8;
    memset(system_buffer, ' ', 10); const char expected_buffer[10] = "AOCI    CI";
    // 1. Write on buffer
    Circular_buffer_write(user_buffer, system_buffer, to_write, size_max, &write_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(4, write_pos, "mismatching on write_pos"));
    // 2. Check buffer
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(system_buffer, expected_buffer, 10), "buffer not as expected"));
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Read with Wrap-around: read on a buffer and check that Wrap-arround work properly
int tester_utest_circular_buffer_4(char* test_name){
    // 0. Initialization
    const char system_buffer[10] = "CIAOCIAOCI"; char user_buffer[10];
    int to_read = 6, size_max = 10, read_pos = 6;
    memset(user_buffer, ' ', 10); const char expected_buffer[10] = "AOCICI    ";
    // 1. Read on buffer
    Circular_buffer_read(system_buffer, user_buffer, to_read, size_max, &read_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(2, read_pos, "mismatching on read_pos"));
    // 2. Check buffer
    //printf("Lettura da buffer (System buffer: %.10s)\n", system_buffer);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(user_buffer, expected_buffer, 10), "buffer not as expected"));
    return 1;
}

// Test 5: Write all empty buffer: Write size_max bytes with empty buffer (write_pos = 0)
int tester_utest_circular_buffer_5(char* test_name){
    // 0. Initialization
    const char user_buffer[10] = "CIAOCIAOCI"; char system_buffer[10];
    int to_write = 10, size_max = 10, write_pos = 0;
    memset(system_buffer, ' ', 10); const char expected_buffer[10] = "CIAOCIAOCI";
    // 1. Write on buffer
    Circular_buffer_write(user_buffer, system_buffer, to_write, size_max, &write_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, write_pos, "mismatching on write_pos"));
    // 2. Check buffer
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(system_buffer, expected_buffer, 10), "buffer not as expected"));
    // 3. Test ok, return 1
    return 1;
}

// Test 6: Read all full buffer: size_max bytes with full buffer (read_pos = 0)
int tester_utest_circular_buffer_6(char* test_name){
    // 0. Initialization
    const char system_buffer[10] = "CIAOCIAOCI"; char user_buffer[10];
    int to_read = 10, size_max = 10, read_pos = 0;
    memset(user_buffer, ' ', 10); const char expected_buffer[10] = "CIAOCIAOCI";
    // 1. Read on buffer
    Circular_buffer_read(system_buffer, user_buffer, to_read, size_max, &read_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, read_pos, "mismatching on read_pos"));
    // 2. Check buffer
    //printf("Lettura da buffer (System buffer: %.10s)\n", system_buffer);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(user_buffer, expected_buffer, 10), "buffer not as expected"));
    return 1;
}

// Test 7: Write all empty buffer: Write size_max bytes with empty buffer (write_pos != 0)
int tester_utest_circular_buffer_7(char* test_name){
    // 0. Initialization
    const char user_buffer[10] = "CIAOCIAOCI"; char system_buffer[10];
    int to_write = 10, size_max = 10, write_pos = 7;
    memset(system_buffer, ' ', 10); const char expected_buffer[10] = "OCIAOCICIA";
    // 1. Write on buffer
    Circular_buffer_write(user_buffer, system_buffer, to_write, size_max, &write_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(7, write_pos, "mismatching on write_pos"));
    // 2. Check buffer
    //printf("Lettura da buffer (User buffer: %.10s)\n", user_buffer);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(system_buffer, expected_buffer, 10), "buffer not as expected"));
    // 3. Test ok, return 1
    return 1;
}

// Test 8: Read all full buffer: size_max bytes with full buffer (read_pos != 0)
int tester_utest_circular_buffer_8(char* test_name){
    // 0. Initialization
    const char system_buffer[10] = "CIAOCIAOCI"; char user_buffer[10];
    int to_read = 10, size_max = 10, read_pos = 6;
    memset(user_buffer, ' ', 10); const char expected_buffer[10] = "AOCICIAOCI";
    // 1. Read on buffer
    Circular_buffer_read(system_buffer, user_buffer, to_read, size_max, &read_pos);
    TESTER_UTEST_CHECK(tester_utest_assert_int(6, read_pos, "mismatching on read_pos"));
    // 2. Check buffer
    //printf("Lettura da buffer (System buffer: %.10s)\n", system_buffer);
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, memcmp(user_buffer, expected_buffer, 10), "buffer not as expected"));
    return 1;
}

