#pragma once
#include "disastrOS_resource.h"

#define TESTER_NOCHECK  -1

// RESOURCES MACRO
#define TESTER_UTEST_ASSERT_RESOURCE_CLEANUP()\
    do{\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));\
    }while(0)

#define TESTER_UTEST_ASSERT_RESOURCE_MEM(M_num_resources, M_num_descriptors, M_message_prefix)\
    do{\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-(M_num_resources), M_message_prefix " _resource_allocator mismatching on expected number of allocated resources"));\
        if((M_num_descriptors) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(M_num_descriptors), M_message_prefix " _descriptor_allocator mismatching on expected number of allocated descriptors"));\
        if((M_num_descriptors) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(M_num_descriptors), M_message_prefix " _descriptor_ptr_allocator mismatching on expected number of allocated descriptors_ptrs"));\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, M_num_resources, M_message_prefix " resources_list mismatching on expected number of resources in the list"));\
    }while(0)

#define TESTER_UTEST_ASSERT_RESOURCE_ALLOC(M_id, M_num_resources, M_num_descriptors, M_message_prefix)\
    do{\
        Resource* M_resource = ResourceList_byId(&resources_list, M_id);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) (M_resource), M_message_prefix " can't retrieve resource from resources_list"));\
        TESTER_UTEST_ASSERT_RESOURCE_MEM((M_num_resources), (M_num_descriptors), M_message_prefix);\
    }while(0)

#define TESTER_UTEST_ASSERT_RESOURCE_ATTRIBUTES(M_id, M_type, M_unlinked, M_onopen, M_onclose, M_onclone, M_read, M_write, M_free, M_message_prefix)\
    do{\
        Resource* M_resource = ResourceList_byId(&resources_list, M_id);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) (M_resource), M_message_prefix " can't retrieve resource from resources_list"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_id), (M_resource)->id, M_message_prefix " mismatch on resource->id"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_unlinked), (M_resource)->unlinked, M_message_prefix " mismatch on resource->unlink"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_type), (M_resource)->type, M_message_prefix " mismatch on resource->type"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_onopen), (M_resource)->VMT.onopen, M_message_prefix " mismatch on resource->VMT.onopen"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_onclose), (M_resource)->VMT.onclose, M_message_prefix " mismatch on resource->VMT.onclose"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_onclone), (M_resource)->VMT.onclone, M_message_prefix " mismatch on resource->VMT.onclone"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_read), (M_resource)->VMT.read, M_message_prefix " mismatch on resource->VMT.read"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_write), (M_resource)->VMT.write, M_message_prefix " mismatch on resource->VMT.write"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_free), (M_resource)->VMT.free, M_message_prefix " mismatch on resource->VMT.free"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listalloc(&(M_resource)->descriptors_ptrs, M_message_prefix " descriptors_ptrs list not initialized"));\
    }while(0)


#define TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(M_pcb, M_num_descriptor, M_total_allocated_descriptor, M_prefix)\
    do{\
        if((M_pcb)){\
            TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_pcb)->descriptors, (M_num_descriptor), M_prefix " error on number of descriptors list of the process"));\
        }\
        if((M_total_allocated_descriptor) > -1){\
            TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(M_total_allocated_descriptor), M_prefix " error on number of total allocated descriptors"));\
            TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(M_total_allocated_descriptor), M_prefix " error on number of total allocated descriptorPtrs"));\
        }\
    }while(0)

#define TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTES(M_fd, M_resource_id, M_pcb, M_flags, M_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_pcb), M_prefix " error on pcb pointer"));\
        Descriptor* M_descriptor = DescriptorList_byFd(&(M_pcb)->descriptors, M_fd);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_descriptor), M_prefix " can't retrieve descriptor from descriptor list"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_pcb), (M_descriptor)->pcb, M_prefix " mismatching on descriptor -> pcb"));\
        Resource* M_resource = ResourceList_byId(&resources_list, (M_resource_id));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_resource), M_prefix " can't retrieve resource from resources_list"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_resource), (M_descriptor)->resource, M_prefix " mismatching on descirptor -> resource"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_fd), (M_descriptor)->fd, M_prefix " mismatching on descriptor -> fd"));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_descriptor)->ptr, M_prefix " mismatching on descriptor -> ptr"));\
        TESTER_UTEST_CHECK(tester_utest_assert_pointer((M_descriptor), (M_descriptor)->ptr->descriptor, M_prefix " mismatching on associated DescriptorPtr -> descriptor"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_flags), (M_descriptor)->flags, M_prefix " mismatching on descriptor -> flags"));\
    }while(0)




// IPC MACRO
#define TESTER_UTEST_IPC_ASSERT_CLEANUP()\
    do{\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max, "_ipc_allocator not empty at the startup"));\
    TESTER_UTEST_ASSERT_RESOURCE_CLEANUP();\
    }while(0)

#define TESTER_UTEST_ASSERT_IPC_MEM(N_num_ipc, M_num_resources, M_num_descriptors, M_message_prefix)\
    do{\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max-(N_num_ipc), M_message_prefix " _ipc_allocator mismatching on expected number of allocated ipcs"));\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-(M_num_resources), M_message_prefix " _resource_allocator mismatching on expected number of allocated resources"));\
        if((M_num_descriptors) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(M_num_descriptors), M_message_prefix " _descriptor_allocator mismatching on expected number of allocated descriptors"));\
        if((M_num_descriptors) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(M_num_descriptors), M_message_prefix " _descriptor_ptr_allocator mismatching on expected number of allocated descriptors_ptrs"));\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, (M_num_resources)+(N_num_ipc), M_message_prefix " resources_list mismatching on expected number of resources in the list"));\
    }while(0)

#define TESTER_UTEST_ASSERT_IPC_ALLOC(M_id, N_num_ipc, M_num_resources, M_num_descriptors, M_message_prefix)\
    do{\
        Ipc* M_ipc = (Ipc*) ResourceList_byId(&resources_list, M_id);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) (M_ipc), M_message_prefix " can't retrieve ipc from resources_list"));\
        TESTER_UTEST_ASSERT_IPC_MEM((N_num_ipc), (M_num_resources), (M_num_descriptors), M_message_prefix);\
    }while(0)

#define TESTER_UTEST_ASSERT_IPC_ATTRIBUTES(M_id, M_sizemax, M_type, M_unlinked, M_onopen, M_onclose, M_onclone, M_read, M_write, M_free, M_message_prefix)\
    do{\
        Ipc* M_ipc = (Ipc*) ResourceList_byId(&resources_list, M_id);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) (M_ipc), M_message_prefix " can't retrieve ipc from resources_list"));\
        TESTER_UTEST_ASSERT_RESOURCE_ATTRIBUTES(M_id, M_type, M_unlinked, M_onopen, M_onclose, M_onclone, M_read, M_write, M_free, M_message_prefix);\
        TESTER_UTEST_CHECK(tester_utest_assert_int(0, (M_ipc)->size, M_message_prefix " mismatch on size attribute"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_sizemax), (M_ipc)->size_max, M_message_prefix " mismatch on ipc->size_max attribute"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_ipc)->waiting_list_read, 0, M_message_prefix " ipc->waiting_list_read uninitialized"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_ipc)->waiting_list_write, 0, M_message_prefix " ipc->waiting_list_write uninitialized"));\
    }while(0)


// FIFO MACRO
#define TESTER_UTEST_FIFO_ASSERT_CLEANUP()\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max, "_fifo_allocator not empty at the startup"));\
        TESTER_UTEST_IPC_ASSERT_CLEANUP();\
    }while(0)

#define TESTER_UTEST_ASSERT_FIFO_MEM(N_num_fifo, N_num_ipc, M_num_resources, M_num_descriptors, M_message_prefix)\
    do{\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-(N_num_fifo), M_message_prefix " _fifo_allocator mismatching on expected number of allocated fifos"));\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max-(N_num_ipc), M_message_prefix " _ipc_allocator mismatching on expected number of allocated ipcs"));\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max-(M_num_resources), M_message_prefix " _resource_allocator mismatching on expected number of allocated resources"));\
        if((M_num_descriptors) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(M_num_descriptors), M_message_prefix " _descriptor_allocator mismatching on expected number of allocated descriptors"));\
        if((M_num_descriptors) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(M_num_descriptors), M_message_prefix " _descriptor_ptr_allocator mismatching on expected number of allocated descriptors_ptrs"));\
        if((M_num_resources) != TESTER_NOCHECK) TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, (M_num_resources)+(N_num_ipc)+(N_num_fifo), M_message_prefix " resources_list mismatching on expected number of resources in the list"));\
    }while(0)

#define TESTER_UTEST_ASSERT_FIFO_ALLOC(M_id, N_num_fifo, N_num_ipc, M_num_resources, M_num_descriptors, M_message_prefix)\
    do{\
        Fifo* M_fifo = (Fifo*) ResourceList_byId(&resources_list, M_id);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) (M_fifo), M_message_prefix " can't retrieve fifo from resources_list"));\
        TESTER_UTEST_ASSERT_FIFO_MEM((N_num_fifo), (N_num_ipc), (M_num_resources), (M_num_descriptors), M_message_prefix);\
    }while(0)


#define TESTER_UTEST_ASSERT_FIFO_ATTRIBUTES(M_id, M_type, M_unlinked, M_onopen, M_onclose, M_onclone, M_read, M_write, M_free, M_message_prefix)\
    do{\
        Fifo* M_fifo = (Fifo*) ResourceList_byId(&resources_list, M_id);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((void*) (M_fifo), M_message_prefix " can't retrieve ipc from resources_list"));\
        TESTER_UTEST_ASSERT_IPC_ATTRIBUTES(M_id, DSOS_PIPE_BUF, M_type, M_unlinked, M_onopen, M_onclose, M_onclone, M_read, M_write, M_free, M_message_prefix);\
        TESTER_UTEST_CHECK(tester_utest_assert_int(0, (M_fifo)->readers_number, M_message_prefix " mismatch on fifo->readers_number attribute"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(0, (M_fifo)->writers_number, M_message_prefix " mismatch on fifo->writers_number attribute"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_fifo)->waiting_list_open_writer, 0, M_message_prefix " fifo->waiting_list_open_writer list not initialized"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_fifo)->waiting_list_open_reader, 0, M_message_prefix " fifo->waiting_list_open_reader list not initialized"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(0, (M_fifo)->read_pos, M_message_prefix " mismatch on fifo->readers_number attribute"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(0, (M_fifo)->write_pos, M_message_prefix " mismatch on fifo->readers_number attribute"));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_fifo)->buffer, M_message_prefix " fifo->buffer not allocated"));\
        for(int i=0; i<DSOS_PIPE_BUF; i++){\
            TESTER_UTEST_CHECK(tester_utest_assert_int(0, (M_fifo)->buffer[i], M_message_prefix "error on fifo->buffer initialization"));\
        }\
    }while(0)
