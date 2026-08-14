#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <assert.h>

#include "tester.h"

// Test 1: Try to close a resource and check descriptor deletion (not unlinked file)
int tester_resource_close_test1(char* test_name){
    // 0. Initialize
    int return_value, resource_id, file_descriptor;
    Resource* resource;
    TESTER_UTEST_CHECK(tester_utest_assert_listsize(&resources_list, 0, "Error, resources_list not empty at the startup"));

    // 1. Create resource
    
    // 2. Check existing of file descriptor on the resource

    // 3. Close resource

    // 4. Check effective deallocation of the descriptor and the resource allocation

    return 1;
}

// Test 2: Apre e crea, unlink, chiude (deve venire distrutta)
// Test 3: Try to close an unlinked file (twice opened) and check if it yet exist and try to check if file descriptor deallocated and check descriptor and after the file deletion
    // Il 3 praticamente è apri il file 2 volte, unlinkalo, e verifica se la prima volta non viene distrutto e la seconda sì.
// Test 4: Try to pass Bad file descriptor (not allocated descriptor)  (negativo e mai aperto)
// Test 5: Try to double close 
