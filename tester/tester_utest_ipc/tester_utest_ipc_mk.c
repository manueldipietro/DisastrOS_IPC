#include "tester.h"

#include "disastrOS_resource.h"
#include "disastrOS_ipc.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include <stdio.h>

// Test 1: Try to make an ipc with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_utest_ipc_mk1(){
    // 0. Initialization
    int return_value, resource_id = 0, size_max = 10;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create the ipc
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));
    // 2. Check ipc allocation and the insert into the resources list
    TESTER_UTEST_ASSERT_IPC_ALLOC(resource_id, 1, 0, 0, "after Ipc_mk");
    // 3. Check the ipc struct attribute value
    TESTER_UTEST_ASSERT_IPC_ATTRIBUTES(resource_id, size_max, DSOS_RESTYPE_IPCBASE, 0, NULL, NULL, NULL, Ipc_read, Ipc_write, Ipc_free, "after Ipc_mk");
    //4. Test ok, return 1
    return 1;
}

// Test 2: Try to make a ipc with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_utest_ipc_mk2(){
    // 0. Initialize
    int return_value, resource_id = 0, size_max = 10;
    Ipc* ipc;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create a test ipc and check this happens
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on first Ipc_mk"));
    TESTER_UTEST_ASSERT_IPC_ALLOC(resource_id, 1, 0, 0, "after first Ipc_mk");
    // 2. Try to create ipc with same resource_id (should return DSOS_EEXIST) and not be allocate
    //    For detect if ipc is allocated modify original resource->type and check it after ipc_mk
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(ipc, "can't retrieve ipc for modify it"));
    ipc->resource.type+= 1;
    return_value = Ipc_mk(resource_id, size_max+10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error on Ipc_mk"));
    TESTER_UTEST_ASSERT_IPC_ALLOC(resource_id, 1, 0, 0, "after second Ipc_mk");
    TESTER_UTEST_ASSERT_IPC_ATTRIBUTES(resource_id, size_max, DSOS_RESTYPE_IPCBASE+1, 0, NULL, NULL, NULL, Ipc_read, Ipc_write, Ipc_free, "after Ipc_mk");
    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL) and try to pass invalid size: negative (should return DSOS_EINVAL)
int tester_utest_ipc_mk3(){
    // 0. Initialization
    int return_value, resource_id,  size_max;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Try to create test ipc and check state of allocation and list allocation (should return DSOS_EINVAL)
    resource_id = -1; size_max = 10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first Ipc_mk (can't detect negative id)"));
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 2. Try to create test ipc and check state of allocation and list allocation (should return DSOS_EINVAL)
    resource_id = DSOS_ANON_RES_STARTID; size_max = 10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during second Ipc_mk (can't detect anonymous id)"));
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 3. Try to create a test ipc with negative size and check this not happens
    resource_id = 5; size_max = -10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during third Ipc_mk (can't detect negative size)"));
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 4. Test ok, return 1
    return 1;
}

// Test 4: Try allocating more ipc than the memory can handle
int tester_utest_ipc_mk4(){
    // 0. Initialization
    int return_value, resource_id,  size_max;
    Ipc* ipc;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();
    // 1. Create MAX_NUM_IPCS
    size_max = 10;
    for(int i=0; i<MAX_NUM_IPCS; i++){
        return_value = Ipc_mk(i, size_max);
        TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during allocation of required ipc"));
        TESTER_UTEST_ASSERT_IPC_ALLOC(i, i+1, 0, 0, "during allocation of required ipc");
    }
    // 2. Create MAX_NUM_IPCS+1 (should return DSOS_ENOMEM) and check this not happens
    resource_id = MAX_NUM_IPCS;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOMEM, return_value, "error during MAX_NUM_IPCS+1 ipc creation"));
    TESTER_UTEST_ASSERT_IPC_MEM(MAX_NUM_IPCS, 0, 0, "after MAX_NUM_IPCS+1 ipc creation ");
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) ipc, "error MAX_NUM_IPCS+1 ipc in resources list"));
    // 3. Test ok, return 1
    return 1;
}