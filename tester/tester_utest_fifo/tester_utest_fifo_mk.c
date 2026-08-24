#include "tester_fifo.h"
#include "tester.h"

#include "disastrOS_fifo.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include <stdio.h>

// Test 1: Try to make a fifo with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_utest_fifo_mk1(char* test_name){
    // 0. Initializze
    int return_value, resource_id;
    Fifo* fifo; Ipc* ipc; Resource* resource;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create the fifo
    resource_id = 0;
    return_value = Fifo_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_fifomk"));
    
    // 2. Check fifo allocation and the insert into the resource list
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-1, "Unallocated Fifo"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max, "_ipc_allocator not empty after mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty after mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty after mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty after mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, ""));
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) fifo, "resource list return null pointer for fifo"));

    // 3. Check attributes value
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->readers_number, "mismatch on readers_number attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->writers_number, "mismatch on writers_number attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_writer, 0, "waiting_list_open_writer list not initialized"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_reader, 0, "waiting_list_open_reader list not initialized"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->read_pos, "mismatch on read_pos attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->write_pos, "mismatch on write_pos attribute"));
    for(int i=0; i<PIPE_BUF; i++){
        TESTER_UTEST_CHECK(tester_utest_assert_int(0, fifo->buffer[i], "error on buffer initialization"));
    }

    ipc = (Ipc*) fifo;
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, ipc->size, "mismatch on size attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(PIPE_BUF, ipc->size_max, "mismatch on size_max attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_read, 0, "waiting_list_read uninitialized"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&ipc->waiting_list_write, 0, "waiting_list_write uninitialized"));

    resource = (Resource*) ipc;
    TESTER_UTEST_CHECK(tester_utest_assert_int(resource_id, resource->id, "mismatch on id attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, resource->unlinked, "mismatch unlink attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_IPCFIFO, resource->type, "mismatch on type attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Fifo_onopen, resource->VMT.onopen, "mismatch VMT.onopen attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Fifo_onclose, resource->VMT.onclose, "mismatch VMT.onclose attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Fifo_read, resource->VMT.read, "mismatch VMT.read attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Fifo_write, resource->VMT.write, "mismatch VMT.write attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Fifo_free, resource->VMT.free, "mismatch on VMT.free attribute"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "descriptor list not initialized"));

    // 4. Test ok, return 1
    return 1;
}

// Test 2: Try to make a fifo with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_utest_fifo_mk2(char* test_name){
    // 0. Initialize
    int return_value, resource_id;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create a test resource and check this happens
    resource_id = 0;
    return_value = Fifo_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on Fifo_mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-1, ""));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max, "_ipc_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, ""));
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) fifo, "resource list return null pointer for ipc"));
    fifo->read_pos = 1;

    // 3. Try to create resource with same resource id (should return DSOS_EEXIST) and check this not happens
    return_value = Fifo_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error on Ipc_mk"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-1, ""));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max, "_ipc_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, ""));
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) fifo, "resource list return null pointer for ipc"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(1, fifo->read_pos, "something unexpected happens to resource (read_pos changed)"));

    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_utest_fifo_mk3(char* test_name){
    // 0. Initialize
    int return_value, resource_id;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Try to create a test resource with negative id and check this not happens 
    resource_id = -1;
    return_value = Fifo_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error on Fifo_mk"));
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 2. Try to create a test resource with anonymous id and check this not happens
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = Fifo_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "error on Fifo_mk"));
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try allocating more fifo than the memory can handle. 
int tester_utest_fifo_mk4(char* test_name){
    // 0. Initialize (and check resources_list length)
    int return_value, resource_id;
    Fifo* fifo;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();

    // 1. Create MAX_NUM_RESOURCE, using directly Resource_alloc(for bypass anonymous id control) 
    for(int i=1; i<MAX_NUM_FIFOS+1; i++){
        resource_id = i;
        fifo = Fifo_alloc(resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) fifo, "Error during allocation of a fifo"));
        List_insert(&resources_list, resources_list.last, (ListItem*) fifo);
        fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, i, "Error, fifo not in the resources_list"));
    }

    // 2. Create MAX_NUM_FIFOS+1 (with id 0 for anonymous test problem) for triggering error (should return DSOS_ENOMEM)
    resource_id = 0;
    return_value = Fifo_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_ENOMEM, return_value, "Error during MAX_NUM_FIFOS+1 fifo creation"));
    fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated((void*) fifo, "Error MAX_NUM_FIFOS+1 allocated or list inserted"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, MAX_NUM_FIFOS, "Error, list size after MAX_NUM_FIFOS+1 allocation"));

    // 3. Test ok, return 1
    return 1;
}