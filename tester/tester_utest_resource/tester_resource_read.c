#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>

// Test 1: Try to read a resource with a bad file descriptor (should return DSOS_EBADFD)
int tester_resource_read1(){
    // 0. Initialization
    int return_value, file_descriptor = 150, count = 10;
    void* buffer = (void*) 1;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Read to a non-existent (should return DSOS_EBADFD)
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_read (can't detect non-existing fd)"));
    // 2. Test ok, return 1
    return 1;
}

// Test 2: Try to pass a null buffer (should return DSOS_EINVAL), a negative count (should return DSOS_EINVAL), count equals to 0 (should return DSOS_ENOSYS);
int tester_resource_read2(){
    // 0. Initialization 
    int return_value, file_descriptor, resource_id = 100, count;
    void* buffer;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open and create (O_CREATE) a test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Pass a null buffer to read (should return DSOS_EINVAL)
    buffer = NULL; count = 100;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during disastrOS_read (can't detect null buffer)"));
    // 3. Pass a negative count to read (should return DSOS_EINVAL)
    buffer = (void*) 1; count = -1;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during disastrOS_read (can't detect negative count)"));
    // 4. Pass count = 0 (should return DSOS_ENOSYS)
    buffer = (void*) 1; count = 0;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_read (can't detect happy path)"));
    // 5. Test ok, return 1
    return 1;
}

// Test 3: Try to call read with valid arguments and verify the operation is not supported on the resource (it's a virtual method not implemented (res->VMT.read->NULL)) (should return DSOS_ENOSYS)
int tester_resource_read3(){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 100, count = 100;
    void* buffer = (void*) 1;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open and create (O_CREATE) a test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Call read with valid arguments, VMT should have NULL entry (should return DSOS_ENOSYS)
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_read (can't detect not implemented read)"));
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Test the various flag assignment scenarios for the test access mode (DSOS_O_RDWR (should return DSOS_EBADFD) and DSOS_O_RDONLY/DSOS_O_WRONLY (should return DSOS_ENOSYS))
int tester_resource_read4(){
    // 0. Initialization
    int return_value, resource_id, file_descriptor, count = 100;
    void* buffer = (void*) 1;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open (RDONLY) a test resource and try to read (should return DSOS_ENOSYS)
    resource_id = 100;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_read (unrecognized DSOS_O_RDONLY)"));
    // 2. Create and open (WRONLY) a test resource and try to read (should return DSOS_EBADFD)
    resource_id = 200;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error during disastrOS_read (unrecognized DSOS_O_WRONLY)"));
    // 3. Create and open (RDWR) a test resource and try to read (should return DSOS_ENOSYS)
    resource_id = 300;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOSYS, return_value, "error during disastrOS_read (unrecognized DSOS_O_RDWR)"));
    // 4. Test ok, return 1
    return 1;
}

// Test 5: Modify the VMT and verify that the method implementation is actually executed
int tester_resource_read5_aux(Descriptor* descriptor, void* buffer, int count){return count;}
int tester_resource_read5(){
    // 0. Initialization
    int return_value, resource_id = 100, file_descriptor, count = 272;
    void* buffer = (void*) 1; Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open and create (O_CREATE) a test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Modify the VMT
    resource = ResourceList_byId(&resources_list, resource_id);
    resource->VMT.read = tester_resource_read5_aux;
    // 3. Invoke read and check that return correct return value
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(count, return_value, "error VMT function has not been executed"));
    // 4. Test ok, return 1
    return 1;
}