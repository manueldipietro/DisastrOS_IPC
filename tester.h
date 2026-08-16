#pragma once

#include "linked_list.h"
#include "pool_allocator.h"

#define TESTER_UTEST_CHECK(condition)\
    do { \
        if(!condition){ \
            return 0; \
        } \
    } while(0)

#define TESTER_UTEST_FAILMSG_SIZE     255
#define TESTER_UTEST_NAME_SIZE        255

extern char TESTER_UTEST_FAILMSG[TESTER_UTEST_FAILMSG_SIZE];
extern char TESTER_UTEST_NAME[TESTER_UTEST_NAME_SIZE];

typedef int (*tester_utest_fn)(char* test_name);

typedef struct tester_utest_list {
    char title[255];
    tester_utest_fn utest_fn;
} tester_utest_list;

int tester_utest_execute(char* test_name, tester_utest_fn utest_fn);
int tester_utest_executelist(tester_utest_list* utest_list, int test_number, char* testing_object);
void tester_utest_print(int status, char* test_name, char* fail_dettails);

int tester_utest_assert_int(int expected_value, int got_value, char* message);
int tester_utest_assert_intge(int expected_value, int got_value, char* message);
int tester_utest_assert_ecode(int expected_value, int got_value, char* message);
int tester_utest_assert_ecodege(int expected_value, int got_value, char* message);


int tester_utest_assert_pointer(void* expected_pointer, void* got_pointer, char* message);
int tester_utest_assert_allocated(void* pointer, char* message);
int tester_utest_assert_notallocated(void* pointer, char* message);

int tester_utest_assert_listsize(ListHead* list_head, int expected_size, char* message);
int tester_utest_assert_poolfreeblock(PoolAllocator* pool, int expected_free_block, char* message);

//void tester_itest();
//void tester_itest_header();
//void tester_itest_footer();