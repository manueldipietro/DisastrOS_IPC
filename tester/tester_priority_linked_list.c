#include "tester_priority_linked_list.h"
#include "tester.h"

#include "priority_linked_list.h"

#include <stdio.h>

int tester_utest_priority_linked_list(){
    int is_all_test_ok = 1;
    tester_utest_list utest_priority_linked_list[] = {
        { .title = "Test priority_linked_list 1: initialization", .utest_fn = tester_utest_priority_linked_list1},
        { .title = "Test priority_linked_list 2: insert", .utest_fn = tester_utest_priority_linked_list2},
        { .title = "Test priority_linked_list 3: detach based on priority", .utest_fn = tester_utest_priority_linked_list3},
        { .title = "Test priority_linked_list 4: detach based on same level fifo", .utest_fn = tester_utest_priority_linked_list4},
        { .title = "Test priority_linked_list 5: detach empty list", .utest_fn = tester_utest_priority_linked_list5},
    };
    printf("Executing utest for priority linked list:\n");
    is_all_test_ok *= tester_utest_executelist(utest_priority_linked_list, sizeof(utest_priority_linked_list)/sizeof(utest_priority_linked_list[0]), "priority linked list") ? 1 : 0;
    return is_all_test_ok;
}

// Test 1: Priority linked list initialization
int tester_utest_priority_linked_list1(char* test_name){
    // 0. Initialization
    PriorityListHead head;
    // 1. Init priority linked list
    PriorityList_init(&head);
    // 2. Check for correct initialization the priority linked list
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, head.size, "mismatching on head->size value"));
    for(int i=0; i<PRIORITY_LEVELS; i++){
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(head.priority_level_list[i]), 0, "uninitialized priority level list"));
    }
    // 3. Test ok, return 1
    return 1;
}

// Test 2: Insert based on priority
int tester_utest_priority_linked_list2(char* test_name){
    // 0. Initialization
    PriorityListHead head; PriorityListItem item[PRIORITY_LEVELS*2];
    PriorityListItem* returned;
    PriorityList_init(&head);

    // 1. Initialize items with priority levels
    for(int i=0; i<PRIORITY_LEVELS; i++){
        item[i].priority = i;
        item[i+PRIORITY_LEVELS].priority = i;
    }

    // 1. Insert two elements for any priority level and check
    for(int i=0; i<PRIORITY_LEVELS*2; i++){
        returned = PriorityList_insert(&head, &item[i]);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on priority list insert"));
        TESTER_UTEST_CHECK(tester_utest_assert_int(i+1, head.size, "mismatching on priority list size"));
        if(i < PRIORITY_LEVELS) TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(head.priority_level_list[i%PRIORITY_LEVELS]), 1, "mismatching on priority level list size"));
        else TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(head.priority_level_list[i%PRIORITY_LEVELS]), 2, "mismatching on priority level list length"));
    }

    return 1;
}

// Test 3: Detach priority based
int tester_utest_priority_linked_list3(char* test_name){
    // 0. Initialization
    PriorityListHead head; PriorityListItem item[PRIORITY_LEVELS*2];
    PriorityListItem* returned;
    PriorityList_init(&head);

    //1. initialize and insert two elements for any priority level
    for(int i=0; i<PRIORITY_LEVELS; i++){
        item[i].priority = i;
        returned = PriorityList_insert(&head, &item[i]);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on priority list insert"));
        item[i+PRIORITY_LEVELS].priority = i;
        returned = PriorityList_insert(&head, &item[i+PRIORITY_LEVELS]);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on priority list insert"));
    }

    // 2. Detach all elements (should be by priority)
    for(int i=0; i<PRIORITY_LEVELS; i++){
        returned = PriorityList_detach(&head);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on list detach"));
        TESTER_UTEST_CHECK(tester_utest_assert_int(i, returned->priority, "mismatching on returned item's priority"));
        returned = PriorityList_detach(&head);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on list detach"));
        TESTER_UTEST_CHECK(tester_utest_assert_int(i, returned->priority, "mismatching on returned item's priority"));
    }

    // 3. Test ok, return
    return 1;
}

// Test 4: Detach FIFO policy respect for same priority level 
int tester_utest_priority_linked_list4(char* test_name){
    // 0. Initialization
    PriorityListHead head; PriorityListItem item[10];
    PriorityListItem* returned;
    PriorityList_init(&head);

    //1. initialize and insert two elements for any priority level
    for(int i=0; i<10; i++){
        item[i].priority = 0;
        returned = PriorityList_insert(&head, &item[i]);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on priority list insert"));
    }

    // 2. Detach all elements (should be by priority)
    for(int i=0; i<10; i++){
        returned = PriorityList_detach(&head);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(returned, "unexpected error on list detach"));
        TESTER_UTEST_CHECK(tester_utest_assert_pointer(&item[i], returned, "mismatching on returned item's order (fifo policy)"));
    }

    // 3. Test ok, return
    return 1;
}

// Test 5: Detach from empty list (should return NULL)
int tester_utest_priority_linked_list5(char* test_name){
    // 0. Initialization
    PriorityListHead head; PriorityListItem* item;
    PriorityList_init(&head);
    // 1. Detach from empty list
    item = PriorityList_detach(&head);
    // 2. Check that detach return NULL
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated(item, "detach return non null pointer"));
    return 1;
}

