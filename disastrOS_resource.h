#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"

// Dichiarazione Forward
typedef struct Resource Resource;

typedef int (*disastros_read_fn)(int fd, void* buffer, int count);
typedef int (*disastros_write_fn)(int fd, const void* buffer, int count);
typedef int (*disastros_resource_free_fn)(Resource* resource);


//QUI VANNO AGGIUNTI I PUNTATORI A FUNZIONI DI DEALLOC E DESTROY

// MI STO RENDENDO CONTO ORA CHE OPEN, CLOSE,UNLINK, DESTROY
// NON GLI SERVE IL POLIMORFISMO
// SERVE A: read, write, alloc e dealloc.
// A make no perchè è specifico del tipo risorsa.
typedef struct {
  disastros_read_fn read;
  disastros_write_fn write;
  disastros_resource_free_fn free;
} Resource_VMT;

struct Resource{
  ListItem list;
  int id;
  int type;
  int unlinked;//Forse trasformarlo in un flags sarebbe più versatile?
  ListHead descriptors_ptrs;
  Resource_VMT VMT;
};

typedef ListHead ResourceList;


void Resource_init();

// Costruttore e distruttore
// Nota: type non è più necessario nel costruttore!
Resource* Resource_alloc(int id);
int Resource_free(Resource* resource);

// Posix interface function.  --DA COMMENTARE
int Resource_open(int resource_id, int flags);
int Resource_read(int fd, void* buffer, int count);
int Resource_write(int fd, const void* buffer, int count);
int Resource_close(int fd);
int Resource_unlink(int resource_id);

//AUX (NO USER)
void Resource_destroy(Resource* resource);


// Funzione di ricerca (bisogna eliminare la ricerca sugli ID anonimi)
Resource* ResourceList_byId(ResourceList* l, int id);

//DEBUG FUNCTIONS
void ResourceList_print(ListHead* l);
