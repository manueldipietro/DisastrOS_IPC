#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>
#include <assert.h>

// Test 1: Try to write a resource with a bad file descriptor (should return DSOS_EBADFD)
int tester_resource_write1(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor = 150, count = 10;
    void* buffer = (void*) 1;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try write to a non-existent (should return DSOS_EBADFD)
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_write (can't detect non-existing fd)"));
    // 2. Test ok, return 1
    return 1;
}

// Test 2: Try to pass a null buffer (should return DSOS_EINVAL), a negative count (should return DSOS_EINVAL), count equals to 0 (should return DSOS_ENOSYS);
int tester_resource_write2(char* test_name){
    // 0. Initialization 
    int return_value, file_descriptor, resource_id = 100, count;
    void* buffer;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open and create (O_CREATE) a test resource
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Try to pass a null buffer to write (should return DSOS_EINVAL)
    buffer = NULL; count = 100;
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during disastrOS_write (can't detect null buffer)"));
    // 3. Try to pass a negative count to write (should return DSOS_EINVAL)
    buffer = (void*) 1; count = -1;
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during disastrOS_write (can't detect negative count)"));
    // 4. Try to pass count = 0 (should return DSOS_ENOSYS)
    buffer = (void*) 1; count = 0;
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_write (can't detect happy path)"));
    // 5. Test ok, return 1
    return 1;
}

// Test 3: Try to call write with valid arguments and verify the operation is not supported on the resource (it's a virtual method not implemented (res->VMT.write->NULL)) (should return DSOS_ENOSYS)
int tester_resource_write3(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 100, count = 100;
    void* buffer = (void*) 1;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open and create (O_CREATE) a test resource
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Invoke write with valid arguments, VMT should have NULL entry (should return DSOS_ENOSYS)
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOSYS, return_value, "error during disastrOS_write (can't detect not implemented write)"));
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Test the various flag assignment scenarios for the test access mode (DSOS_O_RDWR (should return DSOS_EBADFD) and DSOS_O_RDONLY/DSOS_O_WRONLY (should return DSOS_ENOSYS))
int tester_resource_write4(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id, count = 100;
    void* buffer = (void*) 1;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open (RDONLY) a test resource and try to write (should return DSOS_EBADFD)
    resource_id = 100;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_write (unrecognized DSOS_O_RDONLY)"));
    // 2. Create and open (WRONLY) a test resource and try to write (should return DSOS_ENOSYS)
    resource_id = 200;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_write (unrecognized DSOS_O_WRONLY)"));
    // 3. Create and open (RDWR) a test resource and try to write (should return DSOS_ENOSYS)
    resource_id = 300;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_write (unrecognized DSOS_O_WRONLY)"));
    // 4. Test ok, return 1
    return 1;
}

// Test 5: Modify the VMT and verify that the method implementation is actually executed
int tester_resource_write5_aux(Descriptor* descriptor, const void* buffer, int count){return count;}
int tester_resource_write5(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 100, count = 272;
    void* buffer = (char*) 1; Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open and create (O_CREATE) a test resource (should return file descriptor (return_value>=0))
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Modify the VMT
    resource = ResourceList_byId(&resources_list, resource_id);
    resource->VMT.write = tester_resource_write5_aux;
    // 3. Invoke write and check that return correct return value
    return_value = disastrOS_write(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error VMT function has not been executed"));
    // 4. Test ok, return 1
    return 1;
}