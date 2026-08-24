#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "pool_allocator.h"

// Dichiarazione Forward
typedef struct Resource Resource;
typedef struct Descriptor Descriptor;

typedef int (*disastros_onopen_fn)(Descriptor* descriptor);
typedef void (*disastros_onclose_fn)(Descriptor* descriptor);
typedef int (*disastros_read_fn)(Descriptor* descriptor, void* buffer, int count);
typedef int (*disastros_write_fn)(Descriptor* descriptor, const void* buffer, int count);
typedef int (*disastros_free_fn)(Resource* resource);

typedef struct {
  disastros_onopen_fn onopen;
  disastros_onclose_fn onclose;
  disastros_read_fn read;
  disastros_write_fn write;
  disastros_free_fn free;
} Resource_VMT;

struct Resource{
  ListItem list;
  int id;
  int type;
  int unlinked;
  ListHead descriptors_ptrs;
  Resource_VMT VMT;
};

typedef ListHead ResourceList;

// Memory management
void Resource_init();
Resource* Resource_alloc(int id);
int Resource_free(Resource* resource);

// Posix interface function.  --DA COMMENTARE
int Resource_mk(int resource_id);
int Resource_open(int resource_id, int flags);
int Resource_read(int fd, void* buffer, int count);
int Resource_write(int fd, const void* buffer, int count);
int Resource_close(int fd);
int Resource_unlink(int resource_id);

//AUX (For OS)
void Resource_destroy(Resource* resource);
void Resource_setter(Resource* resource, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn);
void Resource_desetter(Resource* resource);

// Funzione di ricerca (bisogna eliminare la ricerca sugli ID anonimi)
Resource* ResourceList_byId(ResourceList* l, int id);

//DEBUG FUNCTIONS
void ResourceList_print(ListHead* l);

PoolAllocator* Resource_allocator_getinfo();