#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <assert.h>

#include "tester.h"

// Test 1: Try to close a resource (not unlinked) and check descriptor deletion
int tester_resource_close1(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create and open resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with contextual creation"));
    file_descriptor = return_value;

    // Non conviene rimetterci tutta la parte della open, diamo per scontato che funzioni avendola già testata.

    // 3. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error in disastrOS_close"));

    // 5. Check the resource after close (still allocated)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "expected resource in the list after disastrOS_close"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "expected resource allocated after disastrOS_close"));
    
    // 4. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "expected list running->descriptors empty"));

    // 5. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "expected list resource->descriptors_ptrs empty"));

    // 6. Test ok, return 1
    return 1;
}

// Test 2: Try to close an unlinked resource and check resource deletion
int tester_resource_close2(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create and open resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with contextual resource creation"));
    file_descriptor = return_value;

    // 2. Unlink resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on unlink resource"));

    // 3. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error in disastrOS_close"));
    
    // 5. Check the resource after close (still allocated)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator expected empty after disastrOS_unlink and disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "expected resources_list empty after disastrOS_unlink and disastrOS_close"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated(resource, "expected resource not allocated after disastrOS_unlink and disastrOS_close"));
    
    // 4. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "expected list running->descriptors empty"));

    // 5. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator expected empty after close"));

    // 6. Test ok, return 1
    return 1;
}

// Test 3: Try to double close a file
int tester_resource_close3(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create and open resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with contextual creation"));
    file_descriptor = return_value;

    // 2. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error in disastrOS_close"));

    // 3. Try to close another time the file descriptor (should return DSOS_EBADFD)
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error in disastrOS_close"));

    // 4. Check the resource after close (still allocated)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "expected resource in the list after disastrOS_close"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "expected resource allocated after disastrOS_close"));
    
    // 5. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "expected list running->descriptors empty"));

    // 6. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "expected list resource->descriptors_ptrs empty"));

    // 7. Test ok, return 1
    return 1;
}

// Test 4: Try to pass bad file descriptor (two cases: negative file descriptor and never opened file descriptor)
int tester_resource_close4(char* test_name){
    // 0. Initialize
    int return_value, file_descriptor;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Try to pass a negative file descriptor (should return DSOS_EBADFD)
    file_descriptor = -10;
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error in disastrOS_close"));

    // 2. Try to pass an unexisting file descriptor (should return DSOS_EBADFD)
    file_descriptor = 23;
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EBADFD, return_value, "error in disastrOS_close"));

    // 3. Check memory
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the end"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the end"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the end"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the end"));

    // 4. Test ok, return 1
    return 1;
}

// Test 5: Try to close an unlinked file (twice opened) and check if it yet exist and try to check if file descriptor deallocated and check descriptor and after the file deletion
int tester_resource_close5(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor, file_descriptor_2;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create and open resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with contextual resource creation"));
    file_descriptor = return_value;

    // 2. Reopen the file
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with second disastrOS_open"));
    file_descriptor_2 = return_value;

    // Retrieve resource because it will be impossible after unlink
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "expected resource allocated after disastrOS_close"));

    // 3. Unlink resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on unlink resource"));

    // 4. Close resource
    return_value = disastrOS_close(file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error in disastrOS_close"));

    // 5. Check the resource after close (still allocated)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "expected resource not in the list after unlink"));
    
    // 6. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "expected list running->descriptors empty"));

    // 7. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "expected list resource->descriptors_ptrs empty"));


    // 4. Close resource
    return_value = disastrOS_close(file_descriptor_2);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error in disastrOS_close"));

    // 5. Check the resource after close (still allocated)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "expected resource in the list after disastrOS_close"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated(resource, "expected resource allocated after disastrOS_close"));
    
    // 6. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "expected list running->descriptors empty"));

    // 7. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator expected empty after close"));

    // 8. Test ok, return 1
    return 1;
}

// Test 6: Try if disastrOS_exit close a resource after process exit
int tester_resource_close6_aux_aux(){
    int resource_id, return_value;

    // 0. Create and open resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with contextual creation"));

    return 1;
}

void tester_resource_close6_aux(){
    disastrOS_exit(tester_resource_close6_aux_aux());
}


int tester_resource_close6(char* test_name){
    // 0. Initialize
    int return_value, resource_id, pid;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Spawn son that will open a resource and exit without close
    disastrOS_spawn(tester_resource_close6_aux, 0);

    // 2. Wait for the son
    pid = disastrOS_wait(0, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, pid, "Error during wait"));
    if(return_value==0) return 0;

    // 5. Check the resource after exit (still allocated)
    resource_id = 0;
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-1, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "expected resource in the list after disastrOS_close"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "expected resource allocated after disastrOS_close"));
    
    // 4. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "expected list running->descriptors empty"));

    // 5. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "expected list resource->descriptors_ptrs empty"));

    // 6. Test ok, return 1
    return 1;
}

// Test 7: Try if disastrOS_exit close and destroy unlinked resource after process exit
int tester_resource_close7_aux_aux(){
    int resource_id, return_value;

    // 0. Create and open resource
    resource_id = 0;
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error on open with contextual creation"));

    // 1. Unlink resource
    return_value = disastrOS_unlink(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on unlink resource"));

    return 1;
}

void tester_resource_close7_aux(){
    disastrOS_exit(tester_resource_close7_aux_aux());
}

int tester_resource_close7(char* test_name){
    // 0. Initialize
    int return_value, resource_id, pid;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Spawn son that will open a resource and exit without close
    disastrOS_spawn(tester_resource_close7_aux, 0);

    // 2. Wait for the son
    pid = disastrOS_wait(0, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, pid, "Error during wait"));
    if(return_value==0) return 0;

    // 5. Check the resource after exit (still allocated)
    resource_id = 0;
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator expected resource allocated after disastrOS_close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "expected resource in the list after disastrOS_close"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_notallocated(resource, "expected resource allocated after disastrOS_close"));
    
    // 4. Check effective deallocation of the descriptor
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator expected empty after close"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "expected list running->descriptors empty"));

    // 5. Check effective deallocation of the descriptor_ptr
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator expected empty after close"));

    // 6. Test ok, return 1
    return 1;
}