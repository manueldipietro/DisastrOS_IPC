#include "disastrOS_IPC.h"
#include "disastrOS_resource.h"

#include <assert.h>
#include <stdio.h>

#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include "linked_list.h"

#define IPC_SIZE sizeof(Ipc)
#define IPC_MEMSIZE (sizeof(Ipc)+sizeof(int))
#define IPC_BUFFER_SIZE MAX_NUM_IPCS*IPC_MEMSIZE

static char _ipc_buffer[IPC_BUFFER_SIZE];
static PoolAllocator _ipc_allocator;

void Ipc_init(){
    int result = PoolAllocator_init(&_ipc_allocator, IPC_SIZE, MAX_NUM_IPCS, _ipc_buffer, IPC_BUFFER_SIZE);
    assert(!result && "Error during IPC Poll Alloator init! Kernel Panic!");
    return;
}

// TODO in resources fare funzione: Resource_set(id, type, read, write, free);
// TODO per questa (ipc) fare: Ipc_set(id, type, read, write, free, size_max)
Ipc* Ipc_alloc(int resource_id, int size_max){    // Si chiama resource_id per ricordarsi che l'id ha namespace globale
    // 1. Allocate IPC, if allocation goes wrong return NULL
    Ipc* ipc = (Ipc*) PoolAllocator_getBlock(&_ipc_allocator);
    if(!ipc)
        return NULL;

    // 2. Fills the fields of the ipc inherited by resource
    Resource* resource = (Resource*) &(ipc->resource);
    (resource->list).prev = (resource->list).next = 0;
    resource->id = resource_id;
    resource->type = DSOS_RESTYPE_IPCBASE;
    resource->unlinked = (resource_id >= DSOS_ANON_RES_STARTID ? 1 : 0);

    // 3. Fills the VMT, read and write are NULL because resource will be a virtual class,
    //(resource->VMT).read = Ipc_read;
    //(resource->VMT).write = Ipc_write;
    (resource->VMT).free = Ipc_free;

    // 4. Initialize the descriptors_ptrs list
    List_init(&resource->descriptors_ptrs);

    // 5. Fills the specific attribute of ipc
    ipc->size = 0;
    ipc->size_max = size_max; 

    // 6. Initializze the waiting lists
    List_init(&ipc->waiting_list_read);
    List_init(&ipc->waiting_list_write);

    // 7. Return pointer to the ipc
    return ipc;
}

// Questa come si potrebber rendere polimorfica? Pensavo che si potrebbe invocare il free di livello inferiore
// e poi mettere un controllo che dealloca solo se nella VMT ci sta lei.
// oppure mettere una specie di setter al contrario. bisogna capirlo bene.
// ho dovuto fare il cambio tipo all'inizio sennò non lo accettava il puntatore a funzione della VMT.
int Ipc_free(Resource* resource){
    Ipc* ipc = (Ipc*) resource;
    assert(ipc->resource.descriptors_ptrs.first==0);
    assert(ipc->resource.descriptors_ptrs.last==0);
    assert(ipc->waiting_list_read.size == 0 && "Fatal error during IPC free (waiting_list_read not empty). Kernel Panic!");
    assert(ipc->waiting_list_write.size == 0 && "Fatal error during IPC free (waiting_list_write not empty). Kernel Panic!");
    return PoolAllocator_releaseBlock(&_ipc_allocator, ipc);
}

int Ipc_mk(int resource_id, int size_max){
    // 1. Check resource_id if is not valid (anonymous or negative)
    if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
        return DSOS_EINVAL;

    // 2. Check if resource (ipc) does not existing
    Ipc* ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    if(ipc) return DSOS_EEXIST;

    // 3. Alloc resource and return DSOS_ENOMEM if out of memory, insert new ipc on resources_list
    ipc = Ipc_alloc(resource_id, size_max);
    if(!ipc) return DSOS_ENOMEM;
    ipc = (Ipc*) List_insert(&resources_list, resources_list.last, (ListItem*) ipc);
    assert(ipc && "Fatal error during ipc mk (list_insert). Kernel Panic!");
  
    // 4. Return DSOS_SUCCESS
    return DSOS_SUCCESS;
}