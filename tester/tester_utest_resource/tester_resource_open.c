#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_constants.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"


#include <stdio.h>

// Test 1: Try to open successfully a resource and check correct allocation of descriptor and descriptor-ptr (should return DSOS_SUCCESS)
int tester_resource_open1(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 27;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create test resource
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_mk");
    // 2. Open test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 3. Check for Descriptor allocation (and descriptor ptr) and attribute 
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 1, 1, "error after disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_RDONLY, "error after disastrOS_open");
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_resource_open2(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open resource with negative id and check state of allocation (should return DSOS_EINVAL)
    resource_id = -5;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_open (can't detect negative id)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Open resource with Anonymous id and check state of allocation (should return DSOS_EINVAL)
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_open (can't detect anonymous id)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass unsupported flags (should return DSOS_EINVAL)
int tester_resource_open3(char* test_name){
    // 0. Initialization
    int return_value, resource_id = 15, flags;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open resource with unsupported flags (set all flags to 1) (should return DSOS_EINVAL)
    flags = ~0;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_open (can't detect not supported flags)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Open resource with unsupported flags (set more significative bit (MSB) to 1) (should return DSOS_EINVAL)
    flags = (1U << (sizeof(int)*8-1));
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_open (can't detect not supported flags (case MSB to 1))"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try to pass illegal combination of flags (DSOS_O_WRONLY|DSOS_O_RDWR e DSOS_O_EXCL without O_CREAT)
int tester_resource_open4(char* test_name){
    // 0. Initialization
    int return_value, resource_id, flags;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open resource with unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR) (should return DSOS_EINVAL)
    resource_id = 15;
    flags = DSOS_O_WRONLY | DSOS_O_RDWR;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_open (can't detect not supported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR))"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Open resource with unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT) (should return DSOS_EINVAL)
    flags = DSOS_O_EXCL;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_open (can't detect not supported flags combination (DSOS_O_EXCL without DSOS_O_CREAT))"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 5: Try to open nonexisting resource (without DSOS_O_CREATE) (should return DSOS_ENOENT)
int tester_resource_open5(char* test_name){
    // 0. Initialization
    int return_value, resource_id = 15, flags = 0;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Open unexisting resource (should return DSOS_ENOENT)
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOENT, return_value, "error during first disastrOS_open (can't detect non-existing resource)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 6: Try to opening an non existing resource with O_CREATE flags (should return DSOS_SUCCESS)
int tester_resource_open6(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 27;;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try to open test resource with O_CREAT flags
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Check for resource allocation and attribute
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 1, "1 error after disastrOS_open");
    TESTER_UTEST_ASSERT_RESOURCE_ATTRIBUTES(resource_id, DSOS_RESTYPE_UNDEFIN, 0, NULL, NULL, NULL, NULL, NULL, Resource_free, "error after disastrOS_open");
    // 3. Check for descriptor and atttribue
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_RDONLY|DSOS_O_CREAT, "error during disastrOS_open");
    // 4. Test ok, return 1
    return 1;
}

// Test 7: Try open an existing resource with DSOS_O_CREATE flags (should return DSOS_SUCCESS)
int tester_resource_open7(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 27;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create test resource (it assumes that disastrOS_mkresource is working correctly because it has already been tested)
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkresource"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_mk");
    // 2. Edit resource test type to ensure it will not be altered
    Resource* resource = ResourceList_byId(&resources_list, resource_id);
    resource->type += 1;
    // 3. Open test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    TESTER_UTEST_ASSERT_RESOURCE_MEM(1, 1, "error after disastrOS_open");
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN+1, resource->type, "something unexpected happen to test resource after disastrOS_open(modified)"));
    // 4. Check for descriptor
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 1, 1, "error after disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_RDONLY|DSOS_O_CREAT, "error after disastrOS_open");
    // 4. Test ok, return 1
    return 1;
}

// Test 8: try to open a non existing resource with DSOS_O_CREAT and DSOS_O_EXCL flags
int tester_resource_open8(char* test_name){
    // 0. Initialization
    int return_value, resource_id = 27, file_descriptor;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try to open test resource with O_CREAT flags
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT|DSOS_O_EXCL);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2. Check for resource creation
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 1, "error after disastrOS_open");
    TESTER_UTEST_ASSERT_RESOURCE_ATTRIBUTES(resource_id, DSOS_RESTYPE_UNDEFIN, 0, NULL, NULL, NULL, NULL, NULL, Resource_free, "error after disastrOS_open");
    // 3. Check for descriptor creation
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 1, 1, "error after disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_RDONLY|DSOS_O_CREAT|DSOS_O_EXCL, "error after disastrOS_open");
    // 4. Test ok, return 1
    return 1;
}

// Test 9: try to open an existing resource with DSOS_O_CREAT and DSOS_O_EXCL flags (should return DSOS_EEXIST)
int tester_resource_open9(char* test_name){
    // 0. Initialization
    int return_value, resource_id = 27;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create test resource (it assumes that disastrOS_mkresource is working correctly because it has already been tested)
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkresource"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_mkresource");
    // 2. Edit resource test type to ensure it will not be altered
    Resource* resource = ResourceList_byId(&resources_list, resource_id);
    resource->type += 1;
    // 3. Try to open test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT|DSOS_O_EXCL);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error during disastrOS_open"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_open");
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN+1, resource->type, "something unexpected happen to test resource after disastrOS_open(modified)"));
    // 4. Check for Descriptor allocation and list insert 
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 0, 0, "error after disastrOS_open");
    // 5. Test ok, return 1
    return 1;
}

// Test 10: Open the same resource, first read-only mode, second in write-only mode and after in read-write mode and check the flags in three case
int tester_resource_open10(char* test_name){
    // 0. Initialization
    int return_value, file_descriptor, resource_id = 30;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkresource"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_mkresource");
    // 2. Open test resource in read-only mode
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during first disastrOS_open"));
    file_descriptor = return_value;
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 1, 1, "1 error during first disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_RDONLY, "error during first disastrOS_open");
    // 3. Open test resource in write-only mode
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during second disastrOS_open"));
    file_descriptor = return_value;
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 2, 2, "error during second disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_WRONLY, "error during second disastrOS_open");
    // 4. Open test resource in read-write mode
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during third disastrOS_open"));
    file_descriptor = return_value;
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, 3, 3, "error during third disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(file_descriptor, resource_id, running, DSOS_O_RDWR, "error during third disastrOS_open");
    // 5. Test ok, return 1
    return 1;
}

// Test 11: Try allocating more resource than the memory can handle using open with DSOS_O_CREATE (should return DSOS_ENOMEM)
int tester_resource_open11(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create MAX_NUM_RESOURCE, using directly Resource_alloc(for bypass control) 
    for(int i=1; i<MAX_NUM_RESOURCES+1; i++){
        resource_id = i;
        resource = Resource_alloc(resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-i, "_resource_allocator expected allocation"));
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "error during allocation of a resource"));
        List_insert(&resources_list, resources_list.last, (ListItem*) resource);
        resource = ResourceList_byId(&resources_list, resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, i, "error, resource not in the resources_list"));
    }
    // 2. Creating MAX_NUM_REsource+1 using disastrOS_open with DSOS_O_CREATE
    resource_id=0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOMEM, return_value, "error during MAX_NUM_RESOURCES+1 resource creation"));
    TESTER_UTEST_ASSERT_RESOURCE_MEM(MAX_NUM_RESOURCES, 0, "after MAX_NUM_RESOURCES+1 resource creation");
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "error MAX_NUM_RESOURCES+1 resource in resources list"));
    // 3. Test ok, return 1
    return 1;
}

// Test 12: Try allocating more descriptor in a process than the memory can handle (should return DSOS_EMFILE)
int tester_resource_open12(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create MAX_NUM_DESCRIPTORS_PER_PROCESS+1 resources
    for(int i=0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS; i++){
        resource_id = i;
        return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during required resources disastrOS_open"));
        TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, i, i, "error after required resource disastrOS_open");
        TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, i, i, "error after required resource disastrOS_open");
    }
    // 3. Open another resource (should return DSOS_EMFILE) (Check for not allocation)
    resource_id = MAX_NUM_DESCRIPTORS_PER_PROCESS;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EMFILE, return_value, "error during last disastrOS_open"));
    TESTER_UTEST_ASSERT_RESOURCE_MEM(MAX_NUM_DESCRIPTORS_PER_PROCESS+1, MAX_NUM_DESCRIPTORS_PER_PROCESS, "error after last disastrOS_open");
    TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(running, MAX_NUM_DESCRIPTORS_PER_PROCESS, MAX_NUM_DESCRIPTORS_PER_PROCESS, "error after required resource disastrOS_open");
    // 4. Test ok, return 1
    return 1;
}

// Test 13: Try allocating more descriptor_ptr in a resource than the memory can handle (should return DSOS_ENFILE)
int tester_resource_open13(char* test_name){
    // 0. Initialization
    // . Test ok, return 1
    return 0;
}