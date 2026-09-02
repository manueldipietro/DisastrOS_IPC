#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "pool_allocator.h"

// Forward declaration
typedef struct Resource Resource;
typedef struct Descriptor Descriptor;

// VMT's function pointer and VMT
typedef int (*disastros_onopen_fn)(Descriptor* descriptor);
typedef void (*disastros_onclose_fn)(Descriptor* descriptor);
typedef void (*disastros_onclone_fn)(Descriptor* descriptor);
typedef int (*disastros_read_fn)(Descriptor* descriptor, void* buffer, int count);
typedef int (*disastros_write_fn)(Descriptor* descriptor, const void* buffer, int count);
typedef int (*disastros_free_fn)(Resource* resource);

typedef struct {
  disastros_onopen_fn onopen;
  disastros_onclose_fn onclose;
  disastros_onclone_fn onclone;
  disastros_read_fn read;
  disastros_write_fn write;
  disastros_free_fn free;
} Resource_VMT;

// Resource base class 
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

// Posix interface function
int Resource_mk(int resource_id);
int Resource_open(int resource_id, int flags);
int Resource_close(int fd);
int Resource_unlink(int resource_id);
int Virtual_read(int fd, void* buffer, int count);
int Virtual_write(int fd, const void* buffer, int count);

// Auxiliary functions
void Resource_destroy(Resource* resource);
void Resource_setter(Resource* resource, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_onclone_fn onclone_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn);
void Resource_desetter(Resource* resource);
Resource* ResourceList_byId(ResourceList* l, int id);

// Debug/tester support functions
void ResourceList_print(ListHead* l);
PoolAllocator* Resource_allocator_getinfo();