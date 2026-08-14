#include "tester_resource.h"

#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>

// Test 1: Try to read a resource with a bad file descriptor (should return DSOS_EBADFD)
int tester_resource_read1(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, count;
    void* buffer;

    // 1. Try read to a non-existent (should return DSOS_EBADFD)
    file_descriptor = 150; count = 10; buffer = (void*) 1;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EBADFD, return_value, "Error, disastrOS_read unrecognize non-existent descriptor"));

    // 2. Test ok, return 1
    return 1;
}

// Test 2: Try to pass a null buffer (should return DSOS_EINVAL), a negative count (should return DSOS_EINVAL), count equals to 0 (should return DSOS_ENOSYS);
int tester_resource_read2(char* test_name){
    // 0. Initialization 
    int return_value, resource_id, file_descriptor, count;
    void* buffer;

    // 1. Open and create (O_CREATE) a test resource (should return file descriptor (return_value>=0))
    resource_id = 100;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
        //PROBLEMA, QUESTO DOVREBBE POTER TORNARE UN VALORE >0 e non necessariamente 0.
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));
    file_descriptor = return_value;

    // 2. Try to pass a null buffer to read (should return DSOS_EINVAL)
    buffer = NULL; count = 100;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EINVAL, return_value, "Error, disastrOS_read unrecognized null buffer"));

    // 3. Try to pass a negative count to read (should return DSOS_EINVAL)
    buffer = (void*) 1; count = -1;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EINVAL, return_value, "Error, disastrOS_read unrecognized negative count"));

    // 4. Try to pass count = 0 (should return DSOS_ENOSYS)
    buffer = (void*) 1; count = 0;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOSYS, return_value, "Error, disastrOS_read unrecognized count=0 as valid"));

    // 5. Test ok, return 1
    return 1;
}

// Test 3: Try to call read with valid arguments and verify the operation is not supported on the resource (it's a virtual method not implemented (res->VMT.read->NULL)) (should return DSOS_ENOSYS)
int tester_resource_read3(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor, count;
    void* buffer;

    // 1. Open and create (O_CREATE) a test resource (should return file descriptor (return_value>=0))
    resource_id = 100;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
        //PROBLEMA, QUESTO DOVREBBE POTER TORNARE UN VALORE >0 e non necessariamente 0.
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));
    file_descriptor = return_value;

    // 2. Invoke read with valid arguments, VMT should have NULL entry (should return DSOS_ENOSYS)
    buffer = (void*) 1; count = 100;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOSYS, return_value, "Error, the read operation does not indicate that it is not implemented"));

    // 3. Test ok, return 1
    return 1;
}

// Test 4: Test the various flag assignment scenarios for the test access mode (DSOS_O_RDWR (should return DSOS_EBADFD) and DSOS_O_RDONLY/DSOS_O_WRONLY (should return DSOS_ENOSYS))
int tester_resource_read4(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor, count;
    void* buffer;

    // 1. Create and open (RDONLY) a test resource and try to read (should return DSOS_ENOSYS)
    resource_id = 100;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
        //PROBLEMA, QUESTO DOVREBBE POTER TORNARE UN VALORE >0 e non necessariamente 0.
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));
    file_descriptor = return_value;
    buffer = (void*) 1; count = 100;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOSYS, return_value, "Error, disastrOS_read unrecognized DSOS_O_RDONLY"));

    // 2. Create and open (WRONLY) a test resource and try to read (should return DSOS_EBADFD)
    resource_id = 200;
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY|DSOS_O_CREAT);
        //PROBLEMA, QUESTO DOVREBBE POTER TORNARE UN VALORE >0 e non necessariamente 0.
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));
    file_descriptor = return_value;
    buffer = (void*) 1; count = 100;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EBADFD, return_value, "Error, disastrOS_read unrecognized DSOS_O_WRONLY"));
    
    // 3. Create and open (RDWR) a test resource and try to read (should return DSOS_ENOSYS)
    resource_id = 300;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
        //PROBLEMA, QUESTO DOVREBBE POTER TORNARE UN VALORE >0 e non necessariamente 0.
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));
    file_descriptor = return_value;
    buffer = (void*) 1; count = 100;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOSYS, return_value, "Error, disastrOS_read unrecognized DSOS_O_RDWR"));

    // 4. Test ok, return 1
    return 1;
}

// Test 5: Modify the VMT and verify that the method implementation is actually executed
int tester_resource_read5_aux(int fd, void* buffer, int count){return count;}
int tester_resource_read5(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor, count;
    void* buffer; Resource* resource;

    // 1. Open and create (O_CREATE) a test resource (should return file descriptor (return_value>=0))
    resource_id = 100;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
        //PROBLEMA, QUESTO DOVREBBE POTER TORNARE UN VALORE >0 e non necessariamente 0.
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));
    file_descriptor = return_value;

    // 2. Modify the VMT
    resource = ResourceList_byId(&resources_list, resource_id);
    resource->VMT.read = tester_resource_read5_aux;

    // 3. Invoke read and check that return correct return value
    buffer = (char*) 1; count = 272;
    return_value = disastrOS_read(file_descriptor, buffer, count);
    TESTER_UTEST_CHECK(tester_utest_assert_int(count, return_value, "Error, the specializing function was not executed"));

    // 4. Test ok, return 1
    return 1;
}