#pragma once

//Test resource mk (Creazione della risorsa (non anonima) e sua conseguente allocazione):
int tester_resource_mk_test1(char* test_name);
int tester_resource_mk_test2(char* test_name);
int tester_resource_mk_test3(char* test_name);
int tester_resource_mk_test4(char* test_name);

/**
    UTEST FOR DISASTROS_OPEN (DisastrOS_Resource):
    Test 1: Open a resource with correct flags
    Test 2: Open a res
 */

int tester_resource_open1(char* test_name);
int tester_resource_open2(char* test_name);
int tester_resource_open3(char* test_name);
int tester_resource_open4(char* test_name);
int tester_resource_open5(char* test_name);
int tester_resource_open6(char* test_name);
int tester_resource_open7(char* test_name);
int tester_resource_open8(char* test_name);
int tester_resource_open9(char* test_name);
int tester_resource_open10(char* test_name);
int tester_resource_open11(char* test_name);
int tester_resource_open12(char* test_name);
int tester_resource_open13(char* test_name);



// Test DisastrOS_unlink (DisastrOS_unlink):
/*
    Test 1: Unlink a resource that is not open by any process and verify that is destroyed properly and return DSOS_SUCCESS
    Test 2: Unlink a resource that is open by at least one process and verify that it is not destroyed
    Test 3: Unlink resources with invalid id and check returned error (EINVAL) (2 case: negative id and anonymous id)
    Test 4: Unlink a resource that doesn't exist and check is returned DSOS_ENOENT
    Test 5: Double unlink of a resource (the second time should return DSOS_ENOENT)
    Test 6: Unlink a resource and try to reuse its id (it should work)
*/
int tester_resource_unlink1(char* test_name);
int tester_resource_unlink2(char* test_name);
int tester_resource_unlink3(char* test_name);
int tester_resource_unlink4(char* test_name);
int tester_resource_unlink5(char* test_name);
int tester_resource_unlink6(char* test_name);

// Test DisastrOS_close

/**
    UTEST FOR DisastrOS_write (DisastrOS_Resource):
    Test 1: Try to write a resource with a bad file descriptor (should return DSOS_EBADFD)
    Test 2: Try to pass a Null buffer (should return DSOS_EINVAL) and to pass a negative count (should return DSOS_EINVAL) (invece ==0 dovrebbe funzionare)
    Test 3: Se invocata correttamente dovrebbe non supportare il metodo sulla risorsa base (la risorsa non ha nessun metodo implementato write/read) (should return DSOS_ENOSYS)
    Test 4: Flags di apertura (deve riconoscere caso RO come negativo e WO-RW come positivo)
    Test 5: Test con write fittizia
    --> Non ha senso testare il caso in cui descriptor->resource è nullo, è un caso limite del kernel con le strutture dati compromesse (se si verifica butti il sistema direttamente)
*/
int tester_resource_write1(char* test_name);
int tester_resource_write2(char* test_name);
int tester_resource_write3(char* test_name);
int tester_resource_write4(char* test_name);
int tester_resource_write5(char* test_name);

/**
    UTEST FOR DisastrOS_read (DisastrOS_Resource):
    Test 1: Try to read a resource with a bad file descriptor (should return DSOS_EBADFD)
    Test 2: Try to pass a Null buffer (should return DSOS_EINVAL) and to pass a negative count (should return DSOS_EINVAL) (invece ==0 dovrebbe funzionare)
    Test 3: Se invocata correttamente dovrebbe non supportare il metodo sulla risorsa base (la risorsa non ha nessun metodo implementato write/read) (should return DSOS_ENOSYS)
    Test 4: Flags di apertura (deve riconoscere caso RO come negativo e WO-RW come positivo)
    Test 5: Test con read fittizia
    --> Non ha senso testare il caso in cui descriptor->resource è nullo, è un caso limite del kernel con le strutture dati compromesse (se si verifica butti il sistema direttamente)
*/
int tester_resource_read1(char* test_name);
int tester_resource_read2(char* test_name);
int tester_resource_read3(char* test_name);
int tester_resource_read4(char* test_name);
int tester_resource_read5(char* test_name);
