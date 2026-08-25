#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "tester.h"
#include "tester_resource.h"
#include "tester_ipc.h"
#include "tester_fifo.h"
#include "tester_circular_buffer.h"
#include "tester_priority_linked_list.h"

#include "disastrOS.h"

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int flags=  DSOS_O_RDWR;
  int fd=disastrOS_open(disastrOS_getpid(), flags);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*/*5*/3);
  }
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  
  // 1. Execute unit tests to test the developed modules (resource, IPC, PIPE/FIFO, MQ)
  //    Using fork in the test suite, we ensure that each unit test has a clean exectuion environment
  //    This avoids implementing setup and tear down functions
  int is_utest_ok = 1;
  //is_utest_ok *= tester_utest_circular_buffer();
  //is_utest_ok *= tester_utest_priority_linked_list();
  //is_utest_ok *= tester_utest_resources();
  //is_utest_ok *= tester_utest_ipc();
  //is_utest_ok *= tester_utest_fifo();
  //is_utest_ok *= tester_utest_pipe();
  //is_utest_ok *= tester_utest_mq();
  //is_utest_ok *= tester_utest_spawnwithfd();
  //if(is_utest_ok) printf("\033[1m[\033[1;92m ALL UTEST IS OK \033[0m\033[1m]\033[0m\n");
  //else printf("\033[1m[\033[1;91mSOME TEST FAIL\033[0m\033[1m]\033[0m\n");
        


  
  // 3. Execute integration test for each module
  //    To limit the size of the output, the user will select the test to be executed
  //    To keep the process image clean and allow further tests to be run without restarting the process
  //    integration tests will be executed on a fork of the base process.

  int choose;
  tester_itest_fn choosen_test[9] ={
    test_itest_resource1_init,
    test_itest_fifo1_init,
    test_itest_fifo2_init,
    test_itest_fifo3_init,
    test_itest_fifo4_init,
    NULL,
    NULL,
    NULL,
    NULL
  };

  while(1){
    printf("Enter a number to select the integration test to run:\n");
    printf("0 - disastrOS shutdown\n");
    printf("1 - Resource test\n");
    printf("2 - FIFO: 1 producer, 1 consumer\n");
    printf("3 - FIFO: 1 producer, N consumer\n");
    printf("4 - FIFO: N producer, 1 consumer\n");
    printf("5 - FIFO: N producer, N consumer\n");
    printf("6 - PIPE: 1 producer, 1 consumer\n");
    printf("7 - PIPE: 1 producer, N consumer\n");
    printf("8 - PIPE: N producer, 1 consumer\n");
    printf("9 - PIPE: N producer, N consumer\n");
    scanf("%d", &choose);
    //printf("\033[H\033[J"); fflush(stdout);

    if(choose == 0) disastrOS_shutdown();
    if(choose < 0 || choose > 9) continue;

    choosen_test[choose-1]();
  }

  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
