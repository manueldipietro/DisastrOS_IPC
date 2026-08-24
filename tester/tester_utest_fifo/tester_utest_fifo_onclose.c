#include "disastrOS_fifo.h"
#include "disastrOS_ipc.h"
#include "disastrOS.h"

// Test 1: Spawn 2 reader and 2 writer, after open process in read mode and check close
// Test 2: Spawn 2 reader and 2 writer, after open process in write and check close
// Test 3: Spawn 1 reader and 1 writer and check that on reader close writer return with DSOS_EPIPE
// Test 4: Spawn 1 reader and 1 writer and check that on writer close reader return with EOF


// Test 1: Spawn 2 reader and 2 writer, after open process in read mode and check close
//int tester_utest_fifo_onclose1_auxR(){return disastrOS_exit(1);}
//int tester_utest_fifo_onclose1_auxW(){return disastrOS_exit(1);}
int tester_utest_fifo_onclose1(char* test_name){
    // 0. Initialize

    // 1. Create fifo

    // 2. Spawn 2 reader and 2 writer

    
    
    return 0;
}

// Test 2: Spawn 2 reader and 2 writer, after open process in write and check close
int tester_utest_fifo_onclose2(char* test_name){
    return 0;
}

// Test 3: Spawn 1 reader and 1 writer and check that on reader close writer return with DSOS_EPIPE
int tester_utest_fifo_onclose3(char* test_name){
    return 0;
}

// Test 4: Spawn 1 reader and 1 writer and check that on writer close reader return with EOF
int tester_utest_fifo_onclose4(char* test_name){
    return 0;
}
