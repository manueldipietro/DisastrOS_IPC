#pragma once
#include "disastrOS_resource.h"

#include "disastrOS_descriptor.h"

// IPC class
typedef struct Ipc{
    Resource resource;
    ListHead waiting_list_read;     // Process that are waiting for read on this resource
    ListHead waiting_list_write;    // Process that are waiting for write on this resource
    int size;
    int size_max;
}Ipc;

// Memory management
void Ipc_init();
Ipc* Ipc_alloc(int id, int size_max);
int Ipc_free(Resource* resource);

// Only for test purpose, this will be a virtual class
int Ipc_mk(int resource_id, int size_max);

// Override function
int Ipc_read(Descriptor* descriptor, void* buffer, int count);
int Ipc_write(Descriptor* descriptor, const void* buffer, int count);

// Auxiliary functions
void Ipc_setter(Ipc* ipc, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_onclone_fn onclone_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn, int size_max);
void Ipc_desetter(Resource* resource);
int Ipc_lock_reader(Ipc* ipc);
int Ipc_lock_writer(Ipc* ipc);
void Ipc_unlock_first_reader(Ipc* ipc);
void Ipc_unlock_first_writer(Ipc* ipc);

// Debug/tester support functions
PoolAllocator* Ipc_allocator_getinfo();