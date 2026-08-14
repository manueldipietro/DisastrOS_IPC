#include "tester_resource.h"

#include "disastrOS_resource.h"

#include "disastrOS.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <assert.h>

#include "tester.h"

// Test 1: Try to open a yet created resource with success
// Test 2: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
// Test 3: Try to pass invalid FLAGS combination (should return DSOS_EINVAL)
// Test 4: Try to open unexisting resource with O_CREATE flags (should create resource and return Filedescriptor)
// Test 5: Try to open existing resource with O_CREATE flags (should return Filedescriptor)
// Test 6: Try to open unexisting resource with O_CREATE|O_EXECL flags (should create resource and return Filedescriptor)
// Test 7: Check flags setting in file descriptor
// Test 8: Try to open unexisting resource without O_CREATE (should return DSOS_ENOENT)
// Test 9: Attempts to open more resources than allowed for a single process (should return ). 
// Test 10: Attempts to open resource more time than allowed for a single resource (should return )
// Test 11: 
// Test 12:
// Test 13:
// Test 14:
// Test 15:

// Test 1: Prova ad aprire una risorsa con successo, controllando la corretta allocazione del descrittore e del puntatore al descrittore
// Test 2: Prova ad aprire una risorsa con id non valido (anonimo o negativo)
// Test 3: Prova a passare i flags non supportati
// Test 4: Prova a passare una combinazione illegale di flags (un test con read_write, un test in cui c'è O_EXECLE senza O_CREATE ed un test con entrambi gli errori)
// Test 5: Prova ad aprire una risorsa non esistente e controlla che ritorni errore
// Test 6: Prova ad aprire una risorsa non esistente con il flags ocreate
// Test 7: Prova ad aprire una risorsa esistente con il flags ocreate
// Test 8: Prova ad aprire una risorsa non esistente con il flags ocreate e o_execl
// Test 9: Prova ad aprire una risorsa esistente con il flags ocreate e o_execl
// Test 10: Apre una risorsa in RW, poi in WR e in RWWR e controlla che i flags siano impostati correttamente
// Test 11: Prova a aprire una risorsa non esisente con il flags ocreate e prova a mandare out of memory (con già il massimo di risorse allocate) l'allocatore, dovrebbe ottenere DSOS_ENOMEM
// Test 12: Prova ad aprire una risorsa quando il processo ha già raggiunto il massimo delle risorse apribili da quel processo, non devono essere allocati ne il puntatore ne il descrittore
// Test 13: Prova ad aprire una risorsa quando la risorsa ha già raggiunto il massimo di aperture consentito, non devono essere allocati ne il puntatore ne il descrittore
// Test 14: Prova ad aprire una risorsa unlinked (deve fallire come risorsa non trovata)
// Test 15: Prova ad aprire una risorsa più volte con lo stesso processo
// Test 16: prova ad aprire una risorsa da due processi diversi


// Test 1:

// Test 2: Try to pass invalid id: negative or anonymous (should return both DSOS_EINVAL)
int tester_resource_open_test1(char* test_name){
    // 0. Initialization

}
// Test 3:
// Test 4: