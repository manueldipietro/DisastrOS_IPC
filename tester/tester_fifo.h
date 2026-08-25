#pragma once

typedef struct Arguments_fifo_packet_reader{
    int reader_id;
    int resource_id;
}Arguments_fifo_packet_reader;

typedef struct Arguments_fifo_packet_writer{
    int writer_id;
    int resource_id;
    int to_send;
}Arguments_fifo_packet_writer;

void test_itest_fifo_packet_reader(void* arguments);
void test_itest_fifo_packet_writer(void* arguments);



int tester_utest_fifo();

void test_itest_fifo1_init();
void test_itest_fifo2_init();
void test_itest_fifo3_init();
void test_itest_fifo4_init();

void tester_utest_fifo_utils_sleeper();
int tester_utest_fifo_utils_reader_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval);
int tester_utest_fifo_utils_writer_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval);

int tester_utest_fifo_mk1(char* test_name);
int tester_utest_fifo_mk2(char* test_name);
int tester_utest_fifo_mk3(char* test_name);
int tester_utest_fifo_mk4(char* test_name);

int tester_utest_fifo_onopen1(char* test_name);
int tester_utest_fifo_onopen2(char* test_name);
int tester_utest_fifo_onopen3(char* test_name);
int tester_utest_fifo_onopen4(char* test_name);
int tester_utest_fifo_onopen5(char* test_name);
int tester_utest_fifo_onopen6(char* test_name);
int tester_utest_fifo_onopen7(char* test_name);
int tester_utest_fifo_onopen8(char* test_name);
int tester_utest_fifo_onopen9(char* test_name);

int tester_utest_fifo_read1(char* test_name);

#define TESTER_UTEST_FIFO_ASSERT_CLEANUP()\
    do{\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max, "_fifo_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Ipc_allocator_getinfo(), Ipc_allocator_getinfo()->size_max, "_ipc_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Resource_allocator_getinfo(), Resource_allocator_getinfo()->size_max, "_resource_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max, "_descriptor_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max, "_descriptor_ptr_allocator not empty at the startup"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "resources_list not empty at the startup"));\
    }while(0)

#define TESTER_UTEST_FIFO_ASSERT_ONOPEN_ROLLBACK(M_resource_id, M_expected_resources, M_expected_descriptors, M_expected_descriptors_onprocess, M_expected_descriptorptr_onresource, M_error_prefix)\
    do{\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Fifo_allocator_getinfo(), Fifo_allocator_getinfo()->size_max-1, M_error_prefix "unallocated fifo"));\
    Fifo* M_fifo = (Fifo*) ResourceList_byId(&resources_list, (M_resource_id));\
    TESTER_UTEST_CHECK(tester_utest_assert_allocated(M_fifo, M_error_prefix "can't retrieve fifo from resources list"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(Descriptor_allocator_getinfo(), Descriptor_allocator_getinfo()->size_max-(M_expected_descriptors), M_error_prefix "_descriptor_allocator descriptor allocated number error"));\
    TESTER_UTEST_CHECK(tester_utest_assert_poolfreeblock(DescriptorPtr_allocator_getinfo(), DescriptorPtr_allocator_getinfo()->size_max-(M_expected_descriptors), M_error_prefix "_descriptor_ptr_allocator descriptor_ptr allocated number error"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, (M_expected_resources), M_error_prefix "mismatching on resources list size"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&running->descriptors, (M_expected_descriptors_onprocess), M_error_prefix "mismatching on descriptors list size"));\
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&M_fifo->ipc.resource.descriptors_ptrs, (M_expected_descriptorptr_onresource), M_error_prefix "mismatching on descriptors ptr list size"));\
    }while(0)

#define TESTER_UTEST_FIFO_ASSERT_OPEN_WAITING(M_pid_open, M_mode, M_expected_readers_number, M_expected_writers_number, M_expected_readers_opener_waiting, M_expected_writers_opener_waiting, M_error_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_expected_readers_number), fifo->readers_number, M_error_prefix "mismatching on fifo readers_number"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_expected_writers_number), fifo->writers_number, M_error_prefix "mismatching on fifo writers_number"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_reader, (M_expected_readers_opener_waiting), M_error_prefix "mismatching on fifo waiting list open reader number"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_writer, (M_expected_writers_opener_waiting), M_error_prefix "mismatching on fifo waiting list open writer number"));\
        PCB* M_pcb = PCB_byPID(M_mode==DSOS_O_RDONLY ? &fifo->waiting_list_open_reader : &fifo->waiting_list_open_writer, (M_pid_open));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(M_pcb, M_error_prefix "error opener not in the waiting list"));\
    }while(0)

// Ragionando idealmente questa macro non dovrebbe fare side_effect, quindi la wait andrebbe chiamata fuori
#define TESTER_UTEST_FIFO_ASSERT_ONOPEN_UNLOCKANDEXITANDWAIT(M_pid_open, M_expected_readers_number, M_expected_writers_number, M_expected_readers_opener_waiting, M_expected_writers_opener_waiting, M_error_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_expected_readers_number), fifo->readers_number, M_error_prefix "mismatching on fifo readers_number"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int((M_expected_writers_number), fifo->writers_number, M_error_prefix "mismatching on fifo writers_number"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_reader, (M_expected_readers_opener_waiting), M_error_prefix "mismatching on fifo waiting list open reader number"));\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&fifo->waiting_list_open_writer, (M_expected_writers_opener_waiting), M_error_prefix "mismatching on fifo waiting list open writer number"));\
        PCB* M_pcb = PCB_byPID(&zombie_list, (M_pid_open));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(M_pcb, M_error_prefix "error opener not in the zombie list"));\
        int M_return_value;\
        int M_return_pid = disastrOS_wait((M_pid_open), &M_return_value);\
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, M_return_pid, M_error_prefix "error on disastrOS_wait"));\
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, M_return_value, M_error_prefix "error on return value from opener"));\
    }while(0)

// Forse qui serve una macro di assert close
