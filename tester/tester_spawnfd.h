#pragma once

#include "tester.h"
#include "disastrOS.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"


int tester_utest_spawnfd();

int tester_utest_spawnfd1(char* test_name);

#define TESTER_UTEST_ASSERT_DESCRIPTOR_MEM(M_pcb, M_num_descriptor, M_total_allocated_descriptor, M_prefix)\
    do{\
        if((M_pcb)){\
            TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_pcb)->descriptors, (M_num_descriptor), M_prefix " error on number of descriptors list of the process"));\
        }\
        if((M_total_allocated_descriptor) > -1){\
            TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(M_total_allocated_descriptor), M_prefix "Error on number of total allocated descriptors"));\
            TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(M_total_allocated_descriptor), M_prefix "Error on number of total allocated descriptorPtrs"));\
        }\
    }while(0)

#define TESTER_UTEST_ASSERT_DESCRIPTOR_ATTRIBUTE(M_fd, M_resource_id, M_pcb, M_flags, M_prefix)\
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
