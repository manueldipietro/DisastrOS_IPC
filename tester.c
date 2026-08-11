#include "tester.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#include <signal.h>

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
                unit_test_printer(0, test_name, "SEGMENTATION FAULT");
                return 0;
            }
        }

        // 3.b Retrieve w_status
        if(WIFEXITED(w_status)){
            w_status = WEXITSTATUS(w_status);
            return w_status;
        }

        return 0;
    }

    // 3. Child process: runs test and exits
    int exit_status = utest_fn(test_name);
    exit(exit_status);
}

void unit_test_printer(int status, char* test_name, char* fail_dettails){
    if(status)
        printf("\033[1m[\033[1;92m OK \033[0m\033[1m]\033[0m");
    else
        printf("\033[1m[\033[1;91mFAIL\033[0m\033[1m]\033[0m");
    printf("  %s\n", test_name);
    if(!status) printf("        -> %s\n", fail_dettails);
    return;
}