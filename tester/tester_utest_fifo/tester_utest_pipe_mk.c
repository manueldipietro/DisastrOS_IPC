#include "tester.h"

#include "disastrOS_fifo.h"
#include "disastrOS.h"

// Test 1: Try to make a pipe with success and check its allocation, list insert and attribute (should return DSOS_SUCCESS)
int tester_utest_pipe_mk1(){
    // 0. Initialization
    int return_value, fd[2];
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create the pipe
    return_value = disastrOS_mkpipe(fd);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error on disastrOS_mkpipe"));
    // 2. Check pipe allocation and the insert into the resource_list
    TESTER_UTEST_ASSERT_PIPE_MEM(1, 0, 0, 2, "after disastrOS_mkpipe");
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "error pipe inserted in resources_list"));
    // 3. Check for descriptor allocation
    TESTER_UTEST_ASSERT_DESCRIPTOR_ANM_ATTRIBUTES(fd[DSOS_PIPE_RD], running, DSOS_O_RDONLY, "error on pipe read descriptor");
    TESTER_UTEST_ASSERT_DESCRIPTOR_ANM_ATTRIBUTES(fd[DSOS_PIPE_WR], running, DSOS_O_WRONLY, "error on pipe write descriptor");  
    return 1;
}

// Test 2: Try to alloc pipe with not available descriptor (use process for semplicity) (will rollback resource)
int tester_utest_pipe_mk2(){
    // 0. Initialize
    int return_value, fd[2], resource_id = 10;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create a resource and open it MAX_NUM_DESCRIPTORS_PER_PROCESS
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during allocation of required resource"));
    for(int i=0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS; i++){
        return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during open of required fifo"));
    }
    TESTER_UTEST_ASSERT_RESOURCE_MEM(1, MAX_NUM_DESCRIPTORS_PER_PROCESS, "error on required descriptors");
    // 2. Try to create a PIPE (should return DSOS_EMFILE)
    return_value = disastrOS_mkpipe(fd);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EMFILE, return_value, "error during disastrOS_mkpipe"));
    // 3. Check for PIPE rollback
    TESTER_UTEST_ASSERT_RESOURCE_MEM(1, MAX_NUM_DESCRIPTORS_PER_PROCESS, "error after disastrOS_mkpipe");
    return 1;
}

// Test 3: Try to alloc pipe with only one available descriptor (use process for semplicity) (will rollback resource)
int tester_utest_pipe_mk3(){
    // 0. Initialize
    int return_value, fd[2], resource_id = 10;
    TESTER_UTEST_FIFO_ASSERT_CLEANUP();
    // 1. Create a resource and open it MAX_NUM_DESCRIPTORS_PER_PROCESS
    return_value = disastrOS_mkresource(resource_id);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_SUCCESS, return_value, "error during allocation of required resource"));
    for(int i=0; i<MAX_NUM_DESCRIPTORS_PER_PROCESS-1; i++){
        return_value = disastrOS_open(resource_id, DSOS_O_RDWR);
        TESTER_UTEST_CHECK(tester_utest_assert_ecodege(DSOS_SUCCESS, return_value, "error during open of required fifo"));
    }
    TESTER_UTEST_ASSERT_RESOURCE_MEM(1, MAX_NUM_DESCRIPTORS_PER_PROCESS-1, "error on required descriptors");
    // 2. Try to create a PIPE (should return DSOS_EMFILE)
    return_value = disastrOS_mkpipe(fd);
    TESTER_UTEST_CHECK(tester_utest_assert_ecode(DSOS_EMFILE, return_value, "error during disastrOS_mkpipe"));
    // 3. Check for PIPE rollback
    TESTER_UTEST_ASSERT_RESOURCE_MEM(1, MAX_NUM_DESCRIPTORS_PER_PROCESS-1, "error after disastrOS_mkpipe");
    return 1;
}
