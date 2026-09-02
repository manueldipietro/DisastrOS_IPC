#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <assert.h>

#include "tester.h"

// Test 1: Try to close a resource (not unlinked) and check descriptor deletion
int tester_resource_close1(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 0;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    // 3. Check the allocation of resource after close (still allocated) and deallocation of descriptor and descriptor->ptr
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_close");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after disastrOS_close (after unlink)");
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to close an unlinked resource and check resource deletion
int tester_resource_close2(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 0;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Unlink resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_unlink"));
    // 3. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    // 4. Check resource, descriptor and descriptor-ptr deallocation
    TESTER_UTEST_ASSERT_RESOURCE_MEM(0, 0, "error after disastrOS_close (after unlink)");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after disastrOS_close (after unlink)");
    // 5. Test ok, return 1
    return 1;
}

// Test 3: Try to double close a file
int tester_resource_close3(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, resource_id = 0;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_close"));
    // 3. Try to close another time the file descriptor (should return DSOS_EBADFD)
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_close"));
    // 4. Check the resource after close (still allocated), and deallocation of descriptor and descriptor->ptr
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_close (after unlink)");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after disastrOS_close (after unlink)");
    // 5. Test ok, return 1
    return 1;
}

// Test 4: Try to pass bad file descriptor (two cases: negative file descriptor and never opened file descriptor)
int tester_resource_close4(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try to pass a negative file descriptor (should return DSOS_EBADFD)
    file_descriptor = -10;
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_close (can't detect negative file descriptor)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Try to pass an unexisting file descriptor (should return DSOS_EBADFD)
    file_descriptor = 23;
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_close (can't detect non-existing file descriptor)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 5: Try to close an unlinked file (twice opened) and check if it yet exist and try to check if file descriptor deallocated and check descriptor and after the file deletion
int tester_resource_close5(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor, file_descriptor_2, resource_id = 0;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during first disastrOS_open"));
    file_descriptor = return_value;
    // 2. Reopen the file
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during second disastrOS_open"));
    file_descriptor_2 = return_value;
    // Retrieve resource because it will be impossible after unlink
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "expected resource allocated after disastrOS_close"));
    // 3. Unlink resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_unlink"));
    // 4. Close first file_descriptor
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during first disastrOS_close"));
    // 5. Check the resource after close (still allocated but not in list)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "expected resource not in the list after unlink"));
    // 6. Check effective deallocation of the descriptor and descriptor pointer
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 1, 1, "error after first disastrOS_close");
    // 5. Close second file_descriptor
    return_value = disastrOS_close(file_descriptor_2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during second disastrOS_close"));
    // 6. Check the resource after close (not allocated) and descriptor and descriptor_ptr
    TESTER_UTEST_ASSERT_RESOURCE_MEM(0, 0, "error after second disastrOS_close");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after second disastrOS_close");
    // 8. Test ok, return 1
    return 1;
}

// Test 6: Try if disastrOS_exit close a resource after process exit
int tester_resource_close6_aux(){
    int return_value, resource_id = 0;
    // 0. Create and open resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    return 1;
}
void tester_resource_close6_child(){
    disastrOS_exit(tester_resource_close6_aux());
}
int tester_resource_close6(char* test_name){
    // 0. Initialize
    int return_value, pid, resource_id = 0;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Spawn child that will open a resource and exit without close, and sleep while child execute
    disastrOS_spawn(tester_aux_sleeper, 0);
    disastrOS_spawn(tester_resource_close6_child, 0);
    disastrOS_sleep(1);
    // 2. Check the resource after exit (still allocated) and before wait
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after child disastrOS_close");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after child disastrOS_close");
    // 3. Wait for the child
    pid = disastrOS_wait(0, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, pid, "error during wait"));
    if(return_value==0) return 0;
    // 6. Test ok, return 1
    return 1;
}

// Test 7: Try if disastrOS_exit close and destroy unlinked resource after process exit
int tester_resource_close7_aux(){
    int return_value, resource_id = 0;
    // 0. Create and open resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    // 1. Unlink resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_unlink"));
    return 1;
}
void tester_resource_close7_child(){
    disastrOS_exit(tester_resource_close7_aux());
}
int tester_resource_close7(char* test_name){
    // 0. Initialize
    int return_value, pid;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Spawn son that will open a resource and exit without close
    disastrOS_spawn(tester_aux_sleeper, 0);
    disastrOS_spawn(tester_resource_close7_child, 0);
    disastrOS_sleep(1);
    // 2. Check memory after exit
    TESTER_UTEST_ASSERT_RESOURCE_MEM(0, 0, "error after child disastrOS_close and disastrOS_unlink");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after child disastrOS_close and disastrOS_unlink");
    // 3. Wait for the son
    pid = disastrOS_wait(0, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, pid, "error during disastrOS_wait"));
    if(return_value==0) return 0;
    // 5. Test ok, return 1
    return 1;
}