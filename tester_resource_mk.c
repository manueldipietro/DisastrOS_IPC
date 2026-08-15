#include "tester_resource.h"

#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>

// Test 1: Try to make a resource with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_resource_mk_test1(char* test_name){
    // 0. Initialize
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error, resources_list not empty at the startup"));

    // 1. Try to create the resource
    resource_id = 0;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during resource creation"));
    
    // 2. Check the allocation of the resource and the list insert
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "Error on allocation or list insert"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "Error, resource not in the resources_list"));

    // 3. Check the resource struct attribute value
    TESTER_UTEST_CHECK(tester_utest_assert_int(resource_id, resource->id, "Error mismatch on resource->id"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, resource->unlinked, "Error mismatch on resource->unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN, resource->type, "Error mismatch on resource->type"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(NULL, resource->VMT.read, "Error mismatch on resource->VMT.read"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(NULL, resource->VMT.write, "Error mismatch on resource->VMT.write"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Resource_free, resource->VMT.free, "Error mismatch on resource->VMT.free"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "Error, descriptor list not initialized"));
    // Note: ListItem has not been checked because it is assumed to work, given that resources_list works.
    
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to make a resource with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_resource_mk_test2(char* test_name){
    // 0. Initialize (and check resources_list length)
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error, resources_list not empty at the startup"));

    // 1. Create test resource and check allocation and list allocation
    resource_id = 0;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during first resource creation"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "Error on allocation or list insert"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "Error, resource not in the resources_list"));

    // 2. Try to create resource with same resource_id (should return DSOS_EEXIST) and not allocate
    //    For detect if resource is allocated modify original resource->type and check it after disastrOS_mk
    resource->type+= 1;
    resource_id = 0;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EEXIST, return_value, "Error during second resource creation"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "Error in the list size (reused id)."));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN+1, resource->type, "Error, something happen to first resource"));

    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_resource_mk_test3(char* test_name){
    // 0. Initialize (and check resources_list length)
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error, resources_list not empty at the startup"));

    // 1. Create test resource and check state of allocation and list allocation (should return DSOS_EINVAL and not allocate / list_append)
    resource_id = -1;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EINVAL, return_value, "Error during resource creation"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "Error, the resource was allocated, but it should not have been (negative id)."));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error in the list size (negative id)."));

    // 2. Create test resource and check state of allocation and list allocation (should return DSOS_EINVAL and not allocate / list_append)
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EINVAL, return_value, "Error during resource creation"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "Error, the resource was allocated, but it should not have been (anonymous id)."));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error in the list size (anonymous id)."));

    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try allocating more resources than the memory can handle. 
int tester_resource_mk_test4(char* test_name){
    // 0. Initialize (and check resources_list length)
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error, resources_list not empty at the startup"));

    // 1. Create MAX_NUM_RESOURCE, using directly Resource_alloc(for bypass anonymous id control) 
    for(int i=1; i<MAX_NUM_RESOURCES+1; i++){
        resource_id = i;
        resource = Resource_alloc(resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "Error during allocation of a resource"));
        List_insert(&resources_list, resources_list.last, (ListItem*) resource);
        resource = ResourceList_byId(&resources_list, resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, i, "Error, resource not in the resources_list"));
    }

    // 2. Create MAX_NUM_RESOURCE+1 (with id 0 for anonymous test problem) for triggering error (should return DSOS_ENOMEM)
    resource_id = 0;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOMEM, return_value, "Error during MAX_NUM_RESOURCES+1 resource creation"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "Error MAX_NUM_RESOURCES+1 allocated or list inserted"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, MAX_NUM_RESOURCES, "Error, list size after MAX_NUM_RESOURCES+1 allocation"));

    // 3. Test ok, return 1
    return 1;
}