#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <assert.h>

#include "tester.h"

int tester_utest_resources(){
    int is_all_test_ok = 1;

    tester_utest_list utest_mk[] = {
        { .title = "Test disastrOS_resource: mk1: successfully", .utest_fn = tester_resource_mk1},
        { .title = "Test disastrOS_resource: mk2: double make", .utest_fn = tester_resource_mk2},
        { .title = "Test disastrOS_resource: mk3: invalid (negative) or anonymous id", .utest_fn = tester_resource_mk3},
        { .title = "Test disastrOS_resource: mk4: allocating more resource the memory capacity", .utest_fn = tester_resource_mk4}
    };

    tester_utest_list utest_open[] = {
        { .title = "Test disastrOS_resource: open1: successfully", .utest_fn = tester_resource_open1},
        { .title = "Test disastrOS_resource: open2: invalid (negative) or anonymous id", .utest_fn = tester_resource_open2},
        { .title = "Test disastrOS_resource: open3: unsupported flags", .utest_fn = tester_resource_open3},
        { .title = "Test disastrOS_resource: open4: illegal combination of flags", .utest_fn = tester_resource_open4},
        { .title = "Test disastrOS_resource: open5: non-existing resource", .utest_fn = tester_resource_open5},
        { .title = "Test disastrOS_resource: open6: non-existing resource with DSOS_O_CREATE", .utest_fn = tester_resource_open6},
        { .title = "Test disastrOS_resource: open7: existing resource with DSOS_O_CREATE", .utest_fn = tester_resource_open7},
        { .title = "Test disastrOS_resource: open8: non-existing resource with DSOS_O_CREATE and DSOS_O_EXCL", .utest_fn = tester_resource_open8},
        { .title = "Test disastrOS_resource: open9: existing resource with DSOS_O_CREATE and DSOS_O_EXCL", .utest_fn = tester_resource_open9},
        { .title = "Test disastrOS_resource: open10: multiple open on same resource with RDONLY, WRONLY and RDWR", .utest_fn = tester_resource_open10},
        { .title = "Test disastrOS_resource: open11: create (O_CREATE) more resource than memory capacity", .utest_fn = tester_resource_open11},
        { .title = "Test disastrOS_resource: open12: allocating more descriptor in a process than memory capacity", .utest_fn = tester_resource_open12},
        { .title = "Test disastrOS_resource: open13: allocation more descriptor_ptr in a process than memory capacity", .utest_fn = tester_resource_open13}
    };

    tester_utest_list utest_unlink[] = {
        { .title = "Test disastrOS_resource: unlink1: unlink not open resource", .utest_fn = tester_resource_unlink1},
        { .title = "Test disastrOS_resource: unlink2: unlink open resource", .utest_fn = tester_resource_unlink2},
        { .title = "Test disastrOS_resource: unlink3: invalid id (negative) or anonymous id", .utest_fn = tester_resource_unlink3},
        { .title = "Test disastrOS_resource: unlink4: non-existing resource", .utest_fn = tester_resource_unlink4},
        { .title = "Test disastrOS_resource: unlink5: double unlink", .utest_fn = tester_resource_unlink5},
        { .title = "Test disastrOS_resource: unlink6: try to reuse id immediately after unlink", .utest_fn = tester_resource_unlink6},
    };

    tester_utest_list utest_close[] = {
        { .title = "Test disastrOS_resource: close1: successfully (not unlinked)", .utest_fn = tester_resource_close1},
        { .title = "Test disastrOS_resource: close2: successfully (unlinked)", .utest_fn = tester_resource_close2},
        { .title = "Test disastrOS_resource: close3: double close file descriptor", .utest_fn = tester_resource_close3},
        { .title = "Test disastrOS_resource: close4: invalid file descriptor (negative and never opened)", .utest_fn = tester_resource_close4},
        { .title = "Test disastrOS_resource: close5: close unlinked resource with multiple open descriptors", .utest_fn = tester_resource_close5},
        { .title = "Test disastrOS_resource: close6: automatic close of descriptor on process exit", .utest_fn = tester_resource_close6},
        { .title = "Test disastrOS_resource: close7: automatic close of descriptor on process exit (with unlinked resource)", .utest_fn = tester_resource_close7},  
    };

    tester_utest_list utest_read[] = {
        { .title = "Test disastrOS_resource: read1: invalid file descriptor", .utest_fn = tester_resource_read1},
        { .title = "Test disastrOS_resource: read2: buffer or count invalid", .utest_fn = tester_resource_read2},
        { .title = "Test disastrOS_resource: read3: read operation not supported", .utest_fn = tester_resource_read3},
        { .title = "Test disastrOS_resource: read4: access mode", .utest_fn = tester_resource_read4},
        { .title = "Test disastrOS_resource: read5: test read operation", .utest_fn = tester_resource_read5},
    };

    tester_utest_list utest_write[] = {
        { .title = "Test disastrOS_resource: write1: invalid file descriptor", .utest_fn = tester_resource_write1},
        { .title = "Test disastrOS_resource: write2: buffer or count invalid", .utest_fn = tester_resource_write2},
        { .title = "Test disastrOS_resource: write3: write operation not supported", .utest_fn = tester_resource_write3},
        { .title = "Test disastrOS_resource: write4: access mode", .utest_fn = tester_resource_write4},
        { .title = "Test disastrOS_resource: write5: test write operation", .utest_fn = tester_resource_write5},
    };

    printf("Executing utest for resource module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "resource_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_open, sizeof(utest_open)/sizeof(utest_open[0]), "resource_open") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_unlink, sizeof(utest_unlink)/sizeof(utest_unlink[0]), "resource_unlink") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_close, sizeof(utest_close)/sizeof(utest_close[0]), "resource_close") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_read, sizeof(utest_read)/sizeof(utest_read[0]), "resource_read") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_write, sizeof(utest_write)/sizeof(utest_write[0]), "resource_write") ? 1 : 0;

    return is_all_test_ok;
}

// Qui ci vanno le funzioni che compongono gli INTEGRATION TEST!


/*
void test_itest_resource1_init(){}

void test_itest_rosource1_son(){

}
*/