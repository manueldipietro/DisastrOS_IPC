#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <assert.h>

#include "tester.h"

// Test 1: Try to close a resource (not unlinked) and check descriptor deletion
int tester_resource_close_test1(char* test_name){
    // 0. Initialize
    

    // 1. Create and open resource
    

    // 2. Check existing of file descriptor on the resource


    // 3. Close resource


    // 4. Check effective deallocation of the descriptor and the resource allocation

    return 1;
}

// Test 2: Try to close an unlinked resource and check resource deletion

// Test 3: Try to close an unlinked file (twice opened) and check if it yet exist and try to check if file descriptor deallocated and check descriptor and after the file deletion

// Test 4: Try to pass bad file descriptor (not allocated descriptor)  (negativo e mai aperto)

// Test 5: Try to double close 

// Test 6: Chiusura automatica chiusura processo

// Test 7: Chiusura automatica e distruzione processo