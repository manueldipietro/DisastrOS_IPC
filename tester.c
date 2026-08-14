#include "tester.h"
#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#include <signal.h>

char TESTER_UTEST_FAILMSG[TESTER_UTEST_FAILMSG_SIZE] = "";
char TESTER_UTEST_NAME[TESTER_UTEST_NAME_SIZE] = "";

int tester_utest_execute(char* test_name, tester_utest_fn utest_fn){
    // TODO: Bisogna capire se in questa sezione vanno disattivati i segnali.   
    // 1. Fork Execution
    pid_t pid;
    pid = fork();
    assert(pid >= 0 && "Fatal error during unit_test fork.");
    
    // 2. Father process
    if(pid){
        // 2.a. Wait for son and retrieves its status
        int w_status;
        pid = waitpid(pid, &w_status, 0);
        assert(pid >= 0 && "Fatal error during unit_test wait!");

        // 2.b. Handles SEGFAULT in child process
        if(WIFSIGNALED(w_status)){
            int term_signal = WTERMSIG(w_status);
            if(term_signal == SIGSEGV){
                tester_utest_print(0, test_name, "SEGMENTATION FAULT");
                return 0;
            }
        }

        // 2.c Retrieve w_status and return it
        if(WIFEXITED(w_status)){
            w_status = WEXITSTATUS(w_status);
            return w_status;
        }

        // 2.d Fallback
        return 0;
    }

    // 3. Child process: runs test and exits
    int exit_status = utest_fn(test_name);
    tester_utest_print(exit_status, test_name, TESTER_UTEST_FAILMSG);
    exit(exit_status);
    return 0;
}

void tester_utest_print(int status, char* test_name, char* fail_dettails){
    if(status)
        printf("\033[1m[\033[1;92m OK \033[0m\033[1m]\033[0m");
    else
        printf("\033[1m[\033[1;91mFAIL\033[0m\033[1m]\033[0m");
    printf("  %s\n", test_name);
    if(!status) printf("        -> %s\n", fail_dettails);
    return;
}

int tester_utest_assert_int(int expected_value, int got_value, char* message){
    if(expected_value == got_value) return 1;
    if(message != NULL)
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "%s - Return value assert: expected %s (%d), got %s (%d)", message,  "DA FARE", expected_value, "DA FARE", got_value);
    else snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Return value assert: expected %s (%d), got %s (%d)", "DA FARE", expected_value, "DA FARE", got_value);
    return 0;
}

int tester_utest_assert_pointer(void* expected_pointer, void* got_pointer, char* message){
    if(expected_pointer == got_pointer) return 1;
    if(message != NULL)
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "%s - Pointer value assert: espected: %p, got %p", message, expected_pointer, got_pointer);
    else snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Pointer value assert: espected: %p, got %p", expected_pointer, got_pointer);
    return 0;
}

int tester_utest_assert_allocated(void* pointer, char* message){
    if(pointer) return 1;
    if(message != NULL)
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "%s - Expected allocation (Expected non null pointer - got null pointer)", message);
    else snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Expected allocation (Expected non null pointer - got null pointer)");
    return 0;
}

int tester_utest_assert_notallocated(void* pointer, char* message){
    if(!pointer) return 1;
    if(message != NULL)
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "%s - Unexpected allocation (Expected null pointer - got non null pointer: %p)", message, pointer);
    else snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected allocation (Expected null pointer - got non null pointer: %p)", pointer);
    return 0;
}

int tester_utest_assert_listsize(ListHead* list_head, int expected_size, char* message){
    if(!list_head){
        if(message!=NULL)
            snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "%s - Unexpected list null pointer", message);
        else snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "Unexpected list null pointer");
        return 0;
    }
    if(list_head->size == expected_size) return 1;
    if(message != NULL)
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "%s - List size assert: expected size %d, got size %d", message, expected_size, list_head->size);
    else snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "List size assert: expected size %d, got size %d", expected_size, list_head->size);
    return 0;
}