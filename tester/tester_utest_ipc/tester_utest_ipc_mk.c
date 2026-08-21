#include "tester_ipc.h"
#include "tester.h"

#include "disastrOS_resource.h"
#include "disastrOS_ipc.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include <stdio.h>

// Test 1: Try to make an ipc with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_utest_ipc_mk1(char* test_name){
    // 0. Initializze
    int return_value, resource_id, size_max;
    Ipc* ipc; Resource* resource;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 1. Try to create the ipc
    resource_id = 0; size_max = 10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));

    // 2. Check ipc allocation and the insert into the resources list
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max-1, ""));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, ""));
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) ipc, "resource list return null pointer for ipc"));

    // 3. Check the resource struct attribute value
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "mismatch on size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(size_max, ipc->size_max, "mismatch on size_max attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "waiting_list_read uninitialized"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "waiting_list_write uninitialized"));

    resource = (Resource*) ipc;
    TESTER_UTEST_CHECK(tester_utest_assert_int(resource_id, resource->id, "mismatch on id attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, resource->unlinked, "mismatch unlink attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_IPCBASE, resource->type, "mismatch on type attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Ipc_read, resource->VMT.read, "mismatch VMT.read attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Ipc_write, resource->VMT.write, "mismatch VMT.write attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Ipc_free, resource->VMT.free, "mismatch on VMT.free attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "descriptor list not initialized"));
    
    //4. Test ok, return 1
    return 1;
}

// Test 2: Try to make a resource with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_utest_ipc_mk2(char* test_name){
    // 0. Initialize
    int return_value, resource_id, size_max;
    Ipc* ipc;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 1. Create a test resource and check this happens
    resource_id = 0; size_max = 10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max-1, ""));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, ""));
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) ipc, "resource list return null pointer for ipc"));

    // 3. Try to create resource with same resource id (should return DSOS_EEXIST) and check this not happens
    return_value = Ipc_mk(resource_id, size_max+10);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error on Ipc_mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max-1, ""));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, ""));
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) ipc, "resource list return null pointer for ipc"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(size_max, ipc->size_max, "something unexpected happens to resource (size_max changed)"));

    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL) and try to pass invalid size: negative (should return DSOS_EINVAL)
int tester_utest_ipc_mk3(char* test_name){
    // 0. Initialize
    int return_value, resource_id,  size_max;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 1. Try to create a test resource with negative id and check this not happens 
    resource_id = -1; size_max = 10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error on Ipc_mk"));
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 2. Try to create a test resource with anonymous id and check this not happens
    resource_id = DSOS_ANON_RES_STARTID; size_max = 10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error on Ipc_mk"));
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 3. Try to create a test resource with negative size and check this not happens
    resource_id = 5; size_max = -10;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error on Ipc_mk"));
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try allocating more resource than the memory can handle
// Fare come quello di resource_mk per conformità
int tester_utest_ipc_mk4(char* test_name){
        // 0. Initialize
    int return_value, resource_id,  size_max;
    Ipc* ipc;
    TESTER_UTEST_IPC_ASSERT_CLEANUP();

    // 1. Create MAX_NUM_IPCS
    size_max = 10;
    for(int i=0; i<MAX_NUM_IPCS; i++){
        return_value = Ipc_mk(i, size_max);
        TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Ipc_mk"));
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator unexpected error"));
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max - (i+1), "_ipc_allocator unexpected error"));
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator unexpected error"));
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator unexpected error"));
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, i+1, "ERRORE SULLA DIMENSIONE DELLA LISTA 1"));
        ipc = (Ipc*) ResourceList_byId(&resources_list, i);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) ipc, "resource list return null pointer for ipc"));
    }

    // 2. Create MAX_NUM_IPCS+1 (should return DSOS_ENOMEM) and check this not happens
    resource_id = MAX_NUM_IPCS;
    return_value = Ipc_mk(resource_id, size_max);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOMEM, return_value, "error on Ipc_mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator unexpected error"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max-MAX_NUM_IPCS, "_ipc_allocator unexpected error"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator unexpected error"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator unexpected error"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, MAX_NUM_IPCS, "ERRORE 2"));
    ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) ipc, "resource list return non null pointer for ipc"));

    // 3. Test ok, return 1
    return 1;
}