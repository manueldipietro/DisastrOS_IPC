#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"
#include "tester.h"

void initFunction(void* args) {  
  // 1. Execute Unit Test
  int is_utest_ok = 1;
  is_utest_ok *= tester_utest_circular_buffer();
  is_utest_ok *= tester_utest_spawnfd();
  is_utest_ok *= tester_utest_resources();
  is_utest_ok *= tester_utest_ipc();
  is_utest_ok *= tester_utest_fifo();
  if(is_utest_ok) printf("\033[1m[\033[1;92m ALL UTEST IS OK \033[0m\033[1m]\033[0m\n");
  else printf("\033[1m[\033[1;91mSOME TEST FAIL\033[0m\033[1m]\033[0m\n");
  // 2. Execute Integration Test, use a jump table
  int choose;
  tester_itest_fn choosen_test[9] ={
    test_itest_resource1_init,
    test_itest_fifo1_init,
    test_itest_fifo2_init,
    test_itest_fifo3_init,
    test_itest_fifo4_init,
    test_itest_pipe1_init,
    test_itest_pipe2_init,
    test_itest_pipe3_init,
    test_itest_pipe4_init
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
    if(choose == 0) break;
    if(choose < 0 || choose > 9){
      printf("Invalid choice\n");
      break;
    }
    //printf("\033[H\033[J"); fflush(stdout);
    tester_itest_execute(choosen_test[choose-1]);
  }
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  printf("Started disastrOS\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
