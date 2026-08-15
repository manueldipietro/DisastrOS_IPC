#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "tester.h"
#include "tester_resource.h"

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

  int u_test_counter;
  // U1. Execute Unit Test for resource syscall
  //    mk syscall test
  tester_utest_execute("Test disastrOS_resource: mk: success", tester_resource_mk_test1);
  tester_utest_execute("Test disastrOS_resource: mk: double make", tester_resource_mk_test2);
  tester_utest_execute("Test disastrOS_resource: mk: invalid or anonymous id", tester_resource_mk_test3);
  tester_utest_execute("Test disastrOS_resource: mk: too much resources", tester_resource_mk_test4);

  //    open syscall test
  //tester_utest_execute("Test disaastrOS_resource: unlink: success ", tester_resource_unlink_test1);


  //    unlink syscall test
  //    close syscall test
  //    read syscall test
  printf("Test resource_read (virtual method):\n");
  u_test_counter = 0;
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: read: ", tester_resource_read1);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: read: ", tester_resource_read2);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: read: ", tester_resource_read3);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: read: ", tester_resource_read4);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: read: ", tester_resource_read5);
  printf("Test corretti: %d/%d\n", u_test_counter, 5);

  //    write syscall test
  printf("Test resource_write (virtual method):\n");
  u_test_counter = 0;
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: write: invalid fd", tester_resource_write1);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: write: buffer or count invalid", tester_resource_write2);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: write: write non implementata", tester_resource_write3);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: write: flags", tester_resource_write4);
  u_test_counter+= tester_utest_execute("Test disastrOS_resource: write: Write fittizia", tester_resource_write5);
  printf("Test corretti: %d/%d\n", u_test_counter, 5);

  tester_utest_execute("Test disastrOS_resource: mk:", tester_resource_mk_test1);
  tester_utest_execute("Test disastrOS_resource: mk:", tester_resource_mk_test2);
  tester_utest_execute("Test disastrOS_resource: mk:", tester_resource_mk_test3);
  tester_utest_execute("Test disastrOS_resource: mk:", tester_resource_mk_test4);

  tester_utest_execute("Test disastrOS_resource: unlink:", tester_resource_unlink1);
  tester_utest_execute("Test disastrOS_resource: unlink:", tester_resource_unlink2);
  tester_utest_execute("Test disastrOS_resource: unlink:", tester_resource_unlink3);
  tester_utest_execute("Test disastrOS_resource: unlink:", tester_resource_unlink4);
  tester_utest_execute("Test disastrOS_resource: unlink:", tester_resource_unlink5);
  tester_utest_execute("Test disastrOS_resource: unlink:", tester_resource_unlink6);

  tester_utest_execute("Test disastrOS_resource: open: 1", tester_resource_open1);
  tester_utest_execute("Test disastrOS_resource: open: 2", tester_resource_open2);
  tester_utest_execute("Test disastrOS_resource: open: 3", tester_resource_open3);
  tester_utest_execute("Test disastrOS_resource: open: 4", tester_resource_open4);
  tester_utest_execute("Test disastrOS_resource: open: 5", tester_resource_open5);
  tester_utest_execute("Test disastrOS_resource: open: 6", tester_resource_open6);
  tester_utest_execute("Test disastrOS_resource: open: 7", tester_resource_open7);
  tester_utest_execute("Test disastrOS_resource: open: 8", tester_resource_open8);
  tester_utest_execute("Test disastrOS_resource: open: 9", tester_resource_open9);
  tester_utest_execute("Test disastrOS_resource: open: 10", tester_resource_open10);
  tester_utest_execute("Test disastrOS_resource: open: 11", tester_resource_open11);
  tester_utest_execute("Test disastrOS_resource: open: 12", tester_resource_open12);
  tester_utest_execute("Test disastrOS_resource: open: 13", tester_resource_open13);
  //tester_utest_execute("Test disastrOS_resource: open: 10", tester_resource_open14);


  // U2. Execute Unit Test for IPC syscall
  //    mk syscall test
  //    open syscall test
  //    unlink syscall test
  //    close syscall test
  //    read syscall test
  //    write syscall test

  // U3. Execute Unit Test for PIPE and FIFO syscall

  // U4. Execute Unit Test for MQ_QUEUE

  // U5. Print Unit Test Summary

  // I1. Integration test for resource
  // I2. Integration test for IPC (Controllare che la sincronizzazione sui read funziona)

  disastrOS_shutdown();


  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  
  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int flags=DSOS_O_CREAT | DSOS_O_RDWR;
    printf("flags: %d\n", flags);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_open(i,flags);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    disastrOS_printStatus();
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  for (int i=0; i<10; ++i) {
    if(i%2==0) disastrOS_unlink(i);
    disastrOS_close(i);
  }
  disastrOS_printStatus();
  printf("shutdown!");
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
