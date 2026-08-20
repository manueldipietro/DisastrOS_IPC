#include "tester_ipc.h"
#include "tester.h"

#include <stdio.h>

// Test 1: Try to make an ipc with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_utest_ipc_mk1(){
    return 0;
}

// Test 2: Try to make a resource with the ID of an already allocated resource (should return DSOS_EEXIST)
int tester_utest_ipc_mk2(){
    return 0;
}

// Test 3: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL) and try to pass invalid size: negative (should return DSOS_EINVAL)
int tester_utest_ipc_mk3(){
    return 0;
}

// Test 4: Try allocating more resource than the memory can handle
// Fare come quello di resource_mk per conformità
int tester_utest_ipc_mk4(){
    return 0;
}