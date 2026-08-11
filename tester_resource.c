#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>

#include "tester.h"

//  0. Prova con allocazione normale.
int tester_resource_mk_test1(char* test_name){
    // 0. Inizializza il tester
    int test_result = 1;
    
    // 1. Prova a creare una risorsa
    int res_id = 0;
    int ret_value = Resource_mk(res_id);
    
    // 2. Controlla il valore di ritorno
    test_result *= (ret_value == DSOS_SUCCESS) ? 1 : 0;
    if(!test_result){
        unit_test_printer(test_result, test_name, "Expected: DSOS_SUCCESS (0), GETTED:");
        return test_result;
    }
    printf("RISULTATO: %d\n", ret_value);

    // 3. Controlla che effettivamente sia stata allocata la risorsa
    Resource* resource = ResourceList_byId(&resources_list, res_id);
    test_result *= resource ? 1 : 0;
    printf("test_result con Resource List By id: %d\n", test_result);
    if(!test_result){
        unit_test_printer(test_result, test_name, "Expected: |Getted: NOT ALLOCATED");
        return test_result;
    }

    // 4. Ritorna il risultato del test
    unit_test_printer(test_result, test_name, "");
    return test_result;
}

