#include "tester.h"

#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>

// Test 1: Unlink a resource that is not open by any process and verify that is destroyed properly and return DSOS_SUCCESS
int tester_resource_unlink1(){
    // 0. Initialization
    int return_value, resource_id = 0;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create test resource
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mkresource"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 0, "error after disastrOS_mkresource");
    // 2. Unlink test resource    
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_unlink"));
    // 3. Check for test resource destruction
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "error resource allocated after destroy"));
    TESTER_UTEST_ASSERT_RESOURCE_MEM(0, 0, "error after destroy");
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Unlink a resource that is open by at least one process and verify that it is not destroyed
int tester_resource_unlink2(){
    // 0. Initialization
    int return_value, resource_id = 0;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open the test resource and retrieve for test
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    TESTER_UTEST_ASSERT_RESOURCE_ALLOC(resource_id, 1, 1, "error after disastrOS_open");
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "can't retrieve resource after disastrOS_open"));
    // 2. Unlink test resource    
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_unlink"));
    // 3. Check that resource is allocated and unlinked setted and has been detached from resources_list
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, resource->unlinked, "mismatching on resource->attribute after disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "error on reosurce allocation after DisastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "error resource in the resources_list after disastrOS_unlink"));
    // 4. Test ok, return 1
    return 1;
}

// Test 3: Unlink resources with invalid id and check returned error (EINVAL) (2 case: negative id and anonymous id)
int tester_resource_unlink3(){
    // 0. Initialization
    int return_value, resource_id;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try unlink with negative id
    resource_id = -1;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_unlink (can't detect negative id)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Try unlink with anonymous id    
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_unlink (can't detect anonymous id)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Unlink a resource that doesn't exist (should return DSOS_ENOENT)
int tester_resource_unlink4(){
    // 0. Initialization
    int return_value, resource_id = 10;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Try unlink non-existing resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOENT, return_value, "error during first disastrOS_unlink (can't detect non-existing resource)"));
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 2. Test ok, return 1
    return 1;
}

// Test 5: Double unlink of a resource (the second unlink should return DSOS_ENOENT)
int tester_resource_unlink5(){
    // 0. Initialization
    int return_value, resource_id = 0;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open the test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "can't retrieve resource after disastrOS_open"));
    // 2. Unlink test resource    
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during first disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, resource->unlinked, "mismatching on resource->attribute after first disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "error on reosurce allocation after first disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "error resource in the resources_list after first disastrOS_unlink"));
    // 3. Re unlink test resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOENT, return_value, "error during second disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, resource->unlinked, "mismatching on resource->attribute after second disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "error on reosurce allocation after second disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "error resource in the resources_list after second disastrOS_unlink"));
    // 4. Test ok, return 1
    return 1;
}

// Test 6: Unlink a resource and try to reuse its id (it should work)
int tester_resource_unlink6(){
    // 0. Initialization
    int return_value, resource_id = 0;
    Resource* resource;
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();
    // 1. Create and open the test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "can't retrieve resource after disastrOS_open"));
    // 2. Unlink test resource    
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during first disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, resource->unlinked, "mismatching on resource->attribute after first disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "error on reosurce allocation after first disastrOS_unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "error resource in the resources_list after first disastrOS_unlink"));
    // 3. Re-create test resource
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "error during disastrOS_mkresource"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-2, "error on reosurce allocation after disastrOS_mkresource"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "error resource not in the resources_list after disastrOS_mkresource"));
    // 4. Test ok, return 1
    return 1;
}