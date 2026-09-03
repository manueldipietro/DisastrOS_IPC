#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"

#include <stdio.h>



int tester_utest_spawnfd1_child_aux(int file_descriptor1, int file_descriptor2, int file_descriptor3){
    // 0. Initialize and retrieve arguments
    int resource_id1=10, resource_id2=20;
    // 1. Check the file descriptors
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 3, 6, "(son) error on descriptors memory status");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor1, resource_id1, running, DSOS_O_RDONLY, "(son) file descriptors 1");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor2, resource_id2, running, DSOS_O_WRONLY, "(son) file descriptors 2");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor3, resource_id2, running, DSOS_O_RDWR,   "(son) file descriptors 3");
    //disastrOS_printStatus();
    // 2. Test ok, exit from the son
    return 1;
}

typedef struct tester_utest_spawnfd1_child_args{int file_descriptor[3];}tester_utest_spawnfd1_child_args;
void tester_utest_spawnfd1_child(void* args){
    int file_descriptor1, file_descriptor2, file_descriptor3;
    file_descriptor1 = ((tester_utest_spawnfd1_child_args*) args)->file_descriptor[0];
    file_descriptor2 = ((tester_utest_spawnfd1_child_args*) args)->file_descriptor[1];
    file_descriptor3 = ((tester_utest_spawnfd1_child_args*) args)->file_descriptor[2];
    disastrOS_exit(tester_utest_spawnfd1_child_aux(file_descriptor1, file_descriptor2, file_descriptor3));
    return;
}

int tester_utest_spawnfd1(char* test_name){
    // 0. Initialization
    int return_value, resource_id1=10, resource_id2=20, file_descriptor1, file_descriptor2, file_descriptor3;
    int son_pid;
    // 1. Create 3 resources and alloc an anonymous resources
    return_value = disastrOS_mkresource(resource_id1);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkresource 1"));
    return_value = disastrOS_mkresource(resource_id2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkresource 2"));

    // 2. Open once the first resource (RDONLY) and twice the second resource (WRONLY - RW) and check that they open correctly
    return_value = disastrOS_open(resource_id1, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open 1"));
    file_descriptor1 = return_value;
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 1, 1, "error on first descriptor");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor1, resource_id1, running, DSOS_O_RDONLY, "error on first descriptor");

    return_value = disastrOS_open(resource_id2, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open 2"));
    file_descriptor2 = return_value;
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 2, 2, "error on second descriptor");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor2, resource_id2, running, DSOS_O_WRONLY, "error on second descriptor");

    return_value = disastrOS_open(resource_id2, DSOS_O_RDWR);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on disastrOS_open 3"));
    file_descriptor3 = return_value;
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 3, 3, "error on third descriptor");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor3, resource_id2, running, DSOS_O_RDWR, "error on third descriptor");

    // 3. Spawn Process
    son_pid = last_pid;
    tester_utest_spawnfd1_child_args args;
    args.file_descriptor[0] = file_descriptor1;
    args.file_descriptor[1] = file_descriptor2;
    args.file_descriptor[2] = file_descriptor3;
    disastrOS_spawn_withfd(tester_utest_spawnfd1_child, (void*) &args);

    // 4. Wait for the son
    son_pid = disastrOS_wait(son_pid, &return_value);
    //disastrOS_printStatus();
    if(return_value != 1) return 0;

    // 5. Check after son exit
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 3, 3, "error on memory status after son exit");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor1, resource_id1, running, DSOS_O_RDONLY, "error on first descriptor after son exit");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor2, resource_id2, running, DSOS_O_WRONLY, "error on first descriptor after son exit");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor3, resource_id2, running, DSOS_O_RDWR,   "error on first descriptor after son exit");

    // 6. Test ok, return 1
    return 1;
}