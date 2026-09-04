#include "tester.h"

#include "disastrOS_fifo.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS.h"

#include <stdio.h>

// Test 1: Try to make a fifo with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_utest_fifo_mk1(){
    // 0. Initialization
    int return_value, resource_id = 0;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create the fifo
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkfifo"));
    // 2. Check fifo allocation and the insert into the resources list
    TESTER_UTEST_ASSERT_FIFO_ALLOC(resource_id, 1, 0, 0, 0, "after disastrOS_mkfifo");
    // 3. Check the fifo struct attribute value
    TESTER_UTEST_ASSERT_FIFO_ATTRIBUTES(resource_id, DSOS_RESTYPE_IPCFIFO, 0, Fifo_onopen, Fifo_onclose, Fifo_onclone, Fifo_read, Fifo_write, Fifo_free, "after disastrOS_mkfifo");
    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to make a fifo with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_utest_fifo_mk2(){
    // 0. Initialize
    int return_value, resource_id = 0;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create a test resource and check this happens
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on first DisastrOS_mkfifo"));
    TESTER_UTEST_ASSERT_FIFO_ALLOC(resource_id, 1, 0, 0, 0, "after first DisastrOS_mkfifo");
    // 2. Try to create fifo with same resource_id (should return DSOS_EEXIST) and not be allocate
    //    For detect if fifo is allocated modify original resource->type and check it after ipc_mk
    Fifo* fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(fifo, "can't retrieve fifo for modify it"));
    fifo->ipc.resource.type+= 1;
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error on second DisastrOS_mkfifo"));
    TESTER_UTEST_ASSERT_FIFO_ALLOC(resource_id, 1, 0, 0, 0, "after second DisastrOS_mkfifo");
    TESTER_UTEST_ASSERT_FIFO_ATTRIBUTES(resource_id, DSOS_RESTYPE_IPCFIFO+1, 0, Fifo_onopen, Fifo_onclose, Fifo_onclone, Fifo_read, Fifo_write, Fifo_free, "after disastrOS_mkfifo");
    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_utest_fifo_mk3(){
    // 0. Initialize
    int return_value, resource_id;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Try to create test fifo and check state of allocation and list allocation (should return DSOS_EINVAL)
    resource_id = -1;
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during first disastrOS_mkfifo (can't detect negative id)"));
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 2. Try to create test fifo and check state of allocation and list allocation (should return DSOS_EINVAL)
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error during second disastrOS_mkfifo (can't detect anonymous id)"));
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try allocating more fifo than the memory can handle. 
int tester_utest_fifo_mk4(){
    // 0. Initialization
    int return_value, resource_id;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create MAX_NUM_FIFOS
    for(int i=0; i<MAX_NUM_FIFOS; i++){
        return_value = disastrOS_mkfifo(i);
        TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during allocation of required fifo"));
        TESTER_UTEST_ASSERT_FIFO_ALLOC(i, i+1, 0, 0, 0, "during allocation of required fifo");
    }
    // 2. Create MAX_NUM_FIFOS+1 (should return DSOS_ENOMEM) and check this not happens
    resource_id = MAX_NUM_FIFOS;
    return_value = disastrOS_mkfifo(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOMEM, return_value, "error during MAX_NUM_FIFOS+1 fifo creation"));
    TESTER_UTEST_ASSERT_FIFO_MEM(MAX_NUM_FIFOS, 0, 0, 0, "after MAX_NUM_FIFOS+1 fifo creation ");
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) fifo, "error MAX_NUM_FIFOS+1 fifo in resources list"));
    // 3. Test ok, return 1
    return 1;
}