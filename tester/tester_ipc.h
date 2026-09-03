#pragma once

/**/
void tester_utest_ipc_utils_sleeper();
int tester_utest_ipc_utils_writer(int resource_id, int count, int expected_write_count, int should_exit, int non_blocking);
int tester_utest_ipc_utils_reader(int resource_id, int count, int expected_read_count, int should_exit, int non_blocking);


//TODO: Mettere parentesi di protezione intorno alle variabili delle MARCO!!
// Macro: check that there is mr_expected_writers waiting in the queue and that there is a specific OCB with mr_pid in the queue
#define TESTER_UTEST_IPC_ASSERT_WRITERS_WAITING(M_ipc, M_expected_waiting_writers, M_pid, M_error_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&M_ipc->waiting_list_write, M_expected_waiting_writers, M_error_prefix " mismatching on ipc waiting_list_write"));\
        PCB* M_pcb = PCB_byPID(&(M_ipc->waiting_list_write), M_pid);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(M_pcb, M_error_prefix " writer not in the ipc waiting list"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, M_pcb->status, M_error_prefix " writer not in waiting status"));\
    }while(0)

#define TESTER_UTEST_IPC_ASSERT_WRITERS_UNLOCKANDEXITANDWAIT(M_ipc, M_expected_waiting_writers, M_pid, M_error_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_ipc)->waiting_list_write, (M_expected_waiting_writers), M_error_prefix " mismatching on ipc waiting_list_write"));\
        PCB* M_pcb = PCB_byPID(&zombie_list, (M_pid));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_pcb), M_error_prefix " writer not in zombie status"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(Zombie, (M_pcb)->status, M_error_prefix " writer not in zombie status"));\
        int M_ret;\
        int M_waitpid = disastrOS_wait(M_pid, &(M_ret));\
        TESTER_UTEST_CHECK(tester_utest_assert_ecode((M_pid), (M_waitpid), M_error_prefix " error during wait"));\
        if((M_ret) == 0) return 0;\
    }while(0)

// Macro: check that there is mr_expected_reader waiting in the queue and that there is a specific PCB with mr_pid in the queue
#define TESTER_UTEST_IPC_ASSERT_READERS_WAITING(M_ipc, M_expected_waiting_readers, M_pid, M_error_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&M_ipc->waiting_list_read, M_expected_waiting_readers, M_error_prefix " mismatching on ipc waiting_list_read"));\
        PCB* M_pcb = PCB_byPID(&(M_ipc->waiting_list_read), M_pid);\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated(M_pcb, M_error_prefix " reader not in the ipc waiting list"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(Waiting, M_pcb->status, M_error_prefix " reader not in waiting status"));\
    }while(0)

#define TESTER_UTEST_IPC_ASSERT_READERS_UNLOCKANDEXITANDWAIT(M_ipc, M_expected_waiting_readers, M_pid, M_error_prefix)\
    do{\
        TESTER_UTEST_CHECK(tester_utest_assert_listsize(&(M_ipc)->waiting_list_read, (M_expected_waiting_readers), M_error_prefix " mismatching on ipc waiting_list_read"));\
        PCB* M_pcb = PCB_byPID(&zombie_list, (M_pid));\
        TESTER_UTEST_CHECK(tester_utest_assert_allocated((M_pcb), M_error_prefix " reader not in zombie status"));\
        TESTER_UTEST_CHECK(tester_utest_assert_int(Zombie, (M_pcb)->status, M_error_prefix " reader not in zombie status"));\
        int M_ret;\
        int M_waitpid = disastrOS_wait(M_pid, &(M_ret));\
        TESTER_UTEST_CHECK(tester_utest_assert_ecode((M_pid), (M_waitpid), M_error_prefix " error during wait"));\
        if((M_ret) == 0) return 0;\
    }while(0)

