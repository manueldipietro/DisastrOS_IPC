#pragma once
#include "disastrOS_resource.h"

#include "linked_list.h"
#include "disastrOS_descriptor.h"


typedef struct Ipc{
    Resource resource;
    
    ListHead waiting_list_read;     // Process that are waiting for read on this resource
    ListHead waiting_list_write;    // Process that are waiting for write on this resource

    int size;
    int size_max;
}Ipc;

// Queste 4 funzioni in realtà non servirebbero veramente alla disastrOS_ipc ma le ho messe per la testabilità
void Ipc_init();
Ipc* Ipc_alloc(int id, int size_max);
int Ipc_free(Resource* resource);
int Ipc_mk(int resource_id, int size_max);

int Ipc_read(Descriptor* descriptor, void* buffer, int count);
int Ipc_write(Descriptor* descriptor, const void* buffer, int count);

int Ipc_close(int fd);

PoolAllocator* Ipc_allocator_getinfo();