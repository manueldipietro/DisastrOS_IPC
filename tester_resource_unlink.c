#include "tester_resource.h"

#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include <stdio.h>

// Test 1: Unlink a resource that is not open by any process and verify that is destroyed properly and return DSOS_SUCCESS
int tester_resource_unlink1(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    Resource* resource;

    // 1. Create test resource
    resource_id = 0;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource creation"));

    // 2. Unlink test resource    
    int old_resources_list_size = resources_list.size;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during unlink resource"));

    // 3. Check for test resource destruction
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) resource, "Error on the resource allocation after destroy"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, old_resources_list_size-1, "Error on the resource list size after destroy"));
    // Can't test effective deallocation in memory

    // 4. Test ok, return 1
    return 1;
}

// Test 2: Unlink a resource that is open by at least one process and verify that it is not destroyed
int tester_resource_unlink2(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    Resource* resource;

    // 1. Create and open the test resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    //QUESTO ASSERT NON VA BENE!
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open"));

    // 2. Unlink test resource    
    int old_resources_list_size = resources_list.size;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during unlink resource"));

    // 3. Check that resource has not been destroyed and the unlink attribute.
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) resource, "Error the resource has been destroyed after destroy"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, old_resources_list_size, "Error on the resource list size after destroy"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, resource->unlinked, "Unlinked attribute not setted"));
    
    // 4. Test ok, return 1
    return 1;
}

// Test 3: Unlink resources with invalid id and check returned error (EINVAL) (2 case: negative id and anonymous id)
int tester_resource_unlink3(char* test_name){
    // 0. Initialization
    int return_value, resource_id;

    // 1. Try unlink with negative id
    resource_id = -1;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EINVAL, return_value, "Error, disastrOS_unlink unrecognize negative descriptor"));

    // 2. Try unlink with anonymous id    
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_EINVAL, return_value, "Error, disastrOS_unlink unrecognize anonymous descriptor"));

    // 3. Test ok, return 1
    return 1;
}

// Test 4: Unlink a resource that doesn't exist and check is returned DSOS_ENOENT
int tester_resource_unlink4(char* test_name){
    // 0. Initialization
    int return_value, resource_id;

    // 1. Try unlink non-existing resource
    resource_id = 10;
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOENT, return_value, "Error, disastrOS_unlink unrecognize non existing resource"));

    // 2. Test ok, return 1
    return 1;
}

// Test 5: Double unlink of a resource (the second time should return DSOS_ENOENT)
int tester_resource_unlink5(char* test_name){
    // 0. Initialization
    int return_value, resource_id;

    // 1. Create and open the test resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    //QUESTO ASSERT NON VA BENE!
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open"));

    // 2. Unlink test resource    
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during first unlink resource"));

    // 3. Re unlink test resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOENT, return_value, "Error during second unlink resource"));
    
    // 4. Test ok, return 1
    return 1;
}

// Test 6: Unlink a resource and try to reuse its id (it should work)
int tester_resource_unlink6(char* test_name){
    // 0. Initialization
    int return_value, resource_id;

    // 1. Create and open the test resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    //QUESTO ASSERT NON VA BENE!
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource open and creation"));

    // 2. Unlink test resource    
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during first unlink resource"));
    
    // 3. Re create test resource
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_SUCCESS, return_value, "Error during test resource re creation"));

    // 4. Test ok, return 1
    return 1;
}