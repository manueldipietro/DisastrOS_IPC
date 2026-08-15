#include "tester_resource.h"

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
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    resource_id = 27;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 2. Try to open test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;

    // 3. Check for Descriptor allocation and list insert 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
    
    // 5. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));

    // 6. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "Error mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "Error mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "Error mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_RDONLY, descriptor->flags, "Error mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "Error mismatch on descriptor->ptr"));

    // 8. Test ok, return 1
    return 1;
}

// Test 2: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_resource_open2(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Open resource with negative id and check state of allocation (should return DSOS_EINVAL)
    resource_id = -5;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "negative id unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with negative id"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with negative id"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with negative id"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with negative id"));

    // 2. Open resource with Anonymous id and check state of allocation (should return DSOS_EINVAL)
    resource_id = DSOS_ANON_RES_STARTID;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "anonymous id unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with anonymous id"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with anonymous id"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with anonymous id"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with anonymous id"));

    // 3. Test ok, return 1
    return 1;
}

// Test 3: Try to pass unsupported flags (should return DSOS_EINVAL)
int tester_resource_open3(char* test_name){
    // 0. Initialization
    int return_value, resource_id, flags;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));
    
    // 1. Open resource with unsupported flags (set all flags to 1) (should return DSOS_EINVAL)
    resource_id = 15;
    flags = ~0;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "unsupported flags (case all flags to 1) id unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with unsupported flags (case all flags to 1)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with unsupported flags (case all flags to 1)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with unsupported flags (case all flags to 1)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with unsupported flags (case all flags to 1)"));

    // 2. Open resource with unsupported flags (set more significative bit to 1) (should return DSOS_EINVAL)
    flags = (1U << (sizeof(int)*8-1));
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "unsupported flags (case MSB to 1) id unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with unsupported flags (case MSB to 1)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with unsupported flags (case MSB to 1)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with unsupported flags (case MSB to 1)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with unsupported flags (case MSB to 1)"));

    // 3. Test ok, return 1
    return 1;
}

// Test 4: Try to pass illegal combination of flags (DSOS_O_WRONLY|DSOS_O_RDWR e DSOS_O_EXCL without O_CREAT)
int tester_resource_open4(char* test_name){
    // 0. Initialization
    int return_value, resource_id, flags;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Open resource with unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR) (should return DSOS_EINVAL)
    resource_id = 15;
    flags = DSOS_O_WRONLY | DSOS_O_RDWR;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR) unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with unsupported flags combination (DSOS_O_WRONLY | DSOS_O_RDWR)"));

    // 2. Open resource with unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT) (should return DSOS_EINVAL)
    flags = DSOS_O_EXCL;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EINVAL, return_value, "unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT) unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT)"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with unsupported flags combination (DSOS_O_EXCL without DSOS_O_CREAT)"));

    // 3. Test ok, return 1
    return 1;
}

// Test 5: Try to open nonexisting resource (without DSOS_O_CREATE) (should return DSOS_ENOENT)
int tester_resource_open5(char* test_name){
    // 0. Initialization
    int return_value, resource_id, flags;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Open unexisting resource (should return DSOS_ENOENT)
    resource_id = 15;
    flags = 0;
    return_value = disastrOS_open(resource_id, flags);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOENT, return_value, "unexisting resource unrecognized by disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "unexpected change on resources_list after disastrOS_open with unexisting resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "unexpected change on _resource_allocator after disastrOS_open with unexisting resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open with unexisting resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open with unexisting resource"));

    // 3. Test ok, return 1
    return 1;
}

// Test 6: Try to opening an unexisting resource with O_CREATE flags (should return DSOS_SUCCESS)
int tester_resource_open6(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Try to open test resource with O_CREAT flags
    resource_id = 27;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;

    // 2. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "resource not in the resources_list"));  
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 3. Check the resource struct attribute value
    TESTER_UTEST_CHECK(tester_utest_assert_int(resource_id, resource->id, "mismatch on resource->id"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, resource->unlinked, "mismatch on resource->unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN, resource->type, "mismatch on resource->type"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(NULL, resource->VMT.read, "mismatch on resource->VMT.read"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(NULL, resource->VMT.write, "mismatch on resource->VMT.write"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Resource_free, resource->VMT.free, "mismatch on resource->VMT.free"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor list not initialized"));

    // 4. Check for Descriptor allocation and list insert 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "descriptor not allocated"));
    
    // 5. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));

    // 6. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_RDONLY, descriptor->flags, "mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "mismatch on descriptor->ptr"));

    // 7. Test ok, return 1
    return 1;
}

// Test 7: Try open an existing resource with DSOS_O_CREATE flags (should return DSOS_SUCCESS)
int tester_resource_open7(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    resource_id = 27;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 2. Edit resource test type to ensure it will not be altered
    resource->type += 1;

    // 3. Try to open test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN+1, resource->type, "something unexpected happen to test resource after disastrOS_open(modified)"));

    // 4. Check for Descriptor allocation and list insert 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
    
    // 5. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));

    // 6. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "Error mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "Error mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "Error mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_RDONLY, descriptor->flags, "Error mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "Error mismatch on descriptor->ptr"));

    // 7. Test ok, return 1
    return 1;
}

// Test 8: try to open an unexisting resource with DSOS_O_CREAT and DSOS_O_EXCL flags
int tester_resource_open8(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Try to open test resource with O_CREAT flags
    resource_id = 27;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT|DSOS_O_EXCL);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;

    // 2. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 1, "resource not in the resources_list"));  
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 3. Check the resource struct attribute value
    TESTER_UTEST_CHECK(tester_utest_assert_int(resource_id, resource->id, "mismatch on resource->id"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(0, resource->unlinked, "mismatch on resource->unlink"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN, resource->type, "mismatch on resource->type"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(NULL, resource->VMT.read, "mismatch on resource->VMT.read"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(NULL, resource->VMT.write, "mismatch on resource->VMT.write"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer(Resource_free, resource->VMT.free, "mismatch on resource->VMT.free"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor list not initialized"));

    // 4. Check for Descriptor allocation and list insert 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "descriptor not allocated"));
    
    // 5. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));

    // 6. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_RDONLY, descriptor->flags, "mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "mismatch on descriptor->ptr"));

    // 7. Test ok, return 1
    return 1;
}

// Test 9: try to open an existing resource with DSOS_O_CREAT and DSOS_O_EXCL flags (should return DSOS_EEXIST)
int tester_resource_open9(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    resource_id = 27;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 2. Edit resource test type to ensure it will not be altered
    resource->type += 1;

    // 3. Try to open test resource
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY|DSOS_O_CREAT|DSOS_O_EXCL);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EEXIST, return_value, "error during disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_RESTYPE_UNDEFIN+1, resource->type, "something unexpected happen to test resource after disastrOS_open(modified)"));

    // 4. Check for Descriptor allocation and list insert 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator unexpected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "descriptor in the running descriptors list"));  
    
    // 5. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator unexpected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "descriptor_ptr in the running descriptors_ptrs list"));      

    // 7. Test ok, return 1
    return 1;
}

// Test 10: Open the same resource, first read-only mode, second in write-only mode and after in read-write mode and check the flags in three case
int tester_resource_open10(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    resource_id = 30;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 2. Try to open test resource in read-only mode
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 2a. Check for correct opening 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-1, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 1, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
    // 2b. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-1, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));
    // 2c. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_RDONLY, descriptor->flags, "mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "Error mismatch on descriptor->ptr"));

    // 3. Try to open test resource in read-only mode
    return_value = disastrOS_open(resource_id, DSOS_O_WRONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 3a. Check for correct opening 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-2, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 2, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
    // 3b. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-2, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 2, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));
    // 3c. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_WRONLY, descriptor->flags, "mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "Error mismatch on descriptor->ptr"));

    // 4. Try to open test resource in read-only mode
    return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
    file_descriptor = return_value;
    // 4a. Check for correct opening 
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-3, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 3, "descriptor not in the running descriptors list"));  
    descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
    // 4b. Check for descriptor_ptr allocation and list insert
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-3, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 3, "descriptor_ptr not in the running descriptors_ptrs list"));      
    descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));
    // 4c. Check descriptor attribute
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) running, (void*) descriptor->pcb, "mismatch on descriptor->pcb"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) resource, (void*) descriptor->resource, "mismatch on descriptor->resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(file_descriptor, descriptor->fd, "mismatch on descriptor->file_descriptor"));
    TESTER_UTEST_CHECK(tester_utest_assert_int(DSOS_O_RDWR, descriptor->flags, "mismatch on descriptor->flags"));
    TESTER_UTEST_CHECK(tester_utest_assert_pointer((void*) descriptorPtr, (void*) descriptor->ptr, "Error mismatch on descriptor->ptr"));

    // 5. Test ok, return 1
    return 1;
}

// Test 11: Try allocating more resource than the memory can handle using open with DSOS_O_CREATE (should return DSOS_ENOMEM)
int tester_resource_open11(char* test_name){
    // 0. Initialization
    int return_value, resource_id;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

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
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENOMEM, return_value, "error during disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, MAX_NUM_RESOURCES, "unexpected change on resources_list after disastrOS_open with unexisting resource"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), 0, "unexpected change on _resource_allocator after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "unexpected change on _descriptor_allocator after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "unexpected change on _descriptor_ptr_allocator after disastrOS_open"));

    // 3. Test ok, return 1
    return 1;
}

// Test 12: Try allocating more descriptor in a process than the memory can handle (should return DSOS_EMFILE)
int tester_resource_open12(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create MAX_NUM_DESCRIPTORS_PER_PROCESS+1 resources
    for(int i=0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS+1; i++){
        resource_id = i;
        return_value = Resource_mk(resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
        resource = ResourceList_byId(&resources_list, resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after disastrOS_make"));
    }

    // 2. Open MAX_NUM_DESCRIPTORS_PER_PROCESS resources
    for(int i=0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS; i++){
        resource_id = i;
        resource = ResourceList_byId(&resources_list, resource_id);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after disastrOS_make"));
        return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
        file_descriptor = return_value;
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(i+1), "_descriptor_allocator expected allocation after disastrOS_open"));
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, (i+1), "descriptor not in the running descriptors list"));  
        descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(i+1), "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 1, "descriptor_ptr not in the running descriptors_ptrs list"));      
        descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));
    }

    // 3. Open another resource (should return DSOS_EMFILE) (Check for not allocation)
    resource_id = MAX_NUM_DESCRIPTORS_PER_PROCESS;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EMFILE, return_value, "error during disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-MAX_NUM_DESCRIPTORS_PER_PROCESS, "_descriptor_allocator unexpected allocation after MAX_NUM_DESCRIPTORS_PER_PROCESS+1° disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, MAX_NUM_DESCRIPTORS_PER_PROCESS, "unexpected length list after..."));  
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-MAX_NUM_DESCRIPTORS_PER_PROCESS, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, 0, "descriptor_ptr in the running descriptors_ptrs list (not expected)"));      

    // 4. Test ok, return 1
    return 1;
}

// Test 13: Try allocating more descriptor_ptr in a resource than the memory can handle (should return DSOS_ENFILE)
int tester_resource_open13_aux_aux(){
    int return_value, resource_id;
    Resource* resource;

    // 1. Try to open the resources the MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE+1 time
    resource_id = 1;
    return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_ENFILE, return_value, "error during disastrOS_open"));

    // 2. Check for correct memory status
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE, "_descriptor_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, 0, "descriptor in the running descriptors list"));  
    
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource using son"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE, "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE, "descriptor_ptr not in the running descriptors_ptrs list"));      

    return 1;
}

void tester_resource_open13_aux(){
    disastrOS_exit(tester_resource_open13_aux_aux());
}

int tester_resource_open13(char* test_name){
    // 0. Initialization
    int return_value, resource_id, file_descriptor;
    Resource* resource; Descriptor* descriptor; DescriptorPtr* descriptorPtr;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));

    // 1. Create test resource (it assumes that disastrOS_mk is working correctly because it has already been tested)
    resource_id = 1;
    return_value = Resource_mk(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during disastrOS_mk"));
    resource = ResourceList_byId(&resources_list, resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(resource, "can't retrieve the resource after open"));

    // 2. Open MAX_NUM_DESCRIPTORS_PTR_PER_RESOURCE using only one process
    TESTER_UTEST_CHECK(tester_utest_assert_int(MAX_NUM_DESCRIPTORS_PER_PROCESS, MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE, "test functionality is not guaranteed if MAX_NUM_DESCRIPTORS_PER_PROCESS != MAX_NUM_DESCRIPTORS_PER_RESOURCES."));
    for(int i=0; i<MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE; i++){
        return_value = disastrOS_open(resource_id, DSOS_O_RDONLY);
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during disastrOS_open"));
        file_descriptor = return_value;
        
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(i+1), "_descriptor_allocator expected allocation after disastrOS_open"));
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, (i+1), "descriptor not in the running descriptors list"));  
        descriptor = DescriptorList_byFd(&running->descriptors, file_descriptor);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptor, "error descriptor not allocated"));
        
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(i+1), "_descriptor_ptr_allocator expected allocation after disastrOS_open"));
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resource->descriptors_ptrs, (i+1), "descriptor_ptr not in the running descriptors_ptrs list"));      
        descriptorPtr = DescriptorPtrList_byDesc(&resource->descriptors_ptrs, descriptor);
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(descriptorPtr, "can't retrieve descriptorPtr from resource->descriptors_ptrs"));
    }

    //Spawn son and wait
    disastrOS_spawn(tester_resource_open13_aux, 0);
    int wait_ret = disastrOS_wait(0, &return_value);
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, wait_ret, "error during son wait"));
    if(wait_ret < 0) return 0;

    // 3. Return return_value with test result (from son)
    return return_value;
}