#include "tester_resource.h"

#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>

// Test 1: Try to make a resource with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_resource_mk1(char* test_name){
    // 0. Initialization
    int return_value, resource_id = 0;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create the resource
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkresource"));
    // 2. Check the allocation of the resource and the resources list insert
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "after disastrOS_mkresource");
    // 3. Check the resource struct attribute value
    TESTER_UTEST_ASSERT_RESOURCE_ATTRIBUTES(resource_id, DSOS_RESTYPE_UNDEFIN, 0, NULL, NULL, NULL, NULL, NULL, Resource_free, "after disastrOS_mkresource");
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to make a resource with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_resource_mk2(char* test_name){
    // 0. Initialization
    int return_value, resource_id = 0;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create test resource and check allocation and list allocation
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on first disastrOS_mkresource"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "after first disastrOS_make");
    // 2. Try to create resource with same resource_id (should return DSOS_EEXIST) and not be allocate
    //    For detect if resource is allocated modify original resource->type and check it after disastrOS_mk
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve resource for modify it"));
    resource->type+= 1;
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error during second resource creation"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "after disastrOS_make");
    TESTER_UTEST_ASSERT_RESOURCE_ATTRIBUTES(resource_id, DSOS_RESTYPE_UNDEFIN+1, 0, NULL, NULL, NULL, NULL, NULL, Resource_free, "after disastrOS_make");
    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_resource_mk3(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try to create test resource and check state of allocation and list allocation (should return DSOS_EINVAL)
    resource_id = -1;
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_mkresource (can't detect negative id)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Try to create test resource and check state of allocation and list allocation (should return DSOS_EINVAL)
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during second disastrOS_mkresource (can't detect anonymous id)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try allocating more resources than the memory can handle. 
int tester_resource_mk4(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create MAX_NUM_RESOURCE, using directly Resource_alloc(for bypass anonymous id control) 
    for(int i=1; i<MAX_NUM_RESOURCES+1; i++){
        resource_id = i;
        resource = Resource_alloc(resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "error during allocation of required resources"));
        List_insert(&resources_list, resources_list.last, (ListItem*) resource);
        TESTER_UTEST_ASSERT_RESOURCE_ALLOC(i, i, 0, "during allocation of required resources");
    }
    // 2. Create MAX_NUM_RESOURCE+1 (with id 0 for anonymous test problem) and triggering error (should return DSOS_ENOMEM)
    resource_id = 0;
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOMEM, return_value, "error during MAX_NUM_RESOURCES+1 resource creation"));
    TESTER_UTEST_ASSERT_RESOURCE_MEM(MAX_NUM_RESOURCES, 0, "after MAX_NUM_RESOURCES+1 resource creation");
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "error MAX_NUM_RESOURCES+1 resource in resources list"));
    // 3. Test ok, return 1
    return 1;
}