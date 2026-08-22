#include "disastrOS_ipc.h"
#include "disastrOS_resource.h"

#include <assert.h>
#include <stdio.h>

#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "disastrOS_pcb.h"

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
    // 0. Validate size_max and resource_id
    assert(resource_id >=0 && "Fatal error during IPC alloc (resource_id negative). Kernel Panic!");
    assert(size_max >=0 && "Fatal error during IPC alloc (size_max negative). Kernel Panic!");
    
    // 1. Allocate IPC, if allocation goes wrong return NULL
    Ipc* ipc = (Ipc*) PoolAllocator_getBlock(&_ipc_allocator);
    if(!ipc)
        return NULL;

    // 2. Fills the fields of the ipc inherited by resource
    //Resource* resource = (Resource*) &(ipc->resource);
    //(resource->list).prev = (resource->list).next = 0;
    //resource->id = resource_id;
    //resource->type = DSOS_RESTYPE_IPCBASE;
    //resource->unlinked = (resource_id >= DSOS_ANON_RES_STARTID ? 1 : 0);

    // 3. Fills the VMT, read and write are NULL because resource will be a virtual class,
    //(resource->VMT).read = Ipc_read;
    //(resource->VMT).write = Ipc_write;
    //(resource->VMT).free = Ipc_free;

    // 4. Initialize the descriptors_ptrs list
    //List_init(&resource->descriptors_ptrs);

    // 5. Fills the specific attribute of ipc
    //ipc->size = 0;
    //ipc->size_max = size_max; 

    // 6. Initializze the waiting lists
    //List_init(&ipc->waiting_list_read);
    //List_init(&ipc->waiting_list_write);

    //
    Ipc_setter(ipc, resource_id, DSOS_RESTYPE_IPCBASE, NULL, NULL, Ipc_read, Ipc_write, Ipc_free, size_max);

    // 7. Return pointer to the ipc
    return ipc;
}

void Ipc_setter(Ipc* ipc, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn, int size_max){
    // 1. 
    Resource* resource = &ipc->resource;
    Resource_setter(resource, resource_id, resource_type, onopen_fn, onclose_fn, read_fn, write_fn, free_fn);
    // 2.
    ipc->size = 0;
    ipc->size_max = size_max;    
    // 3.
    List_init(&ipc->waiting_list_read);
    List_init(&ipc->waiting_list_write);
    return;
}

// Questa come si potrebber rendere polimorfica? Pensavo che si potrebbe invocare il free di livello inferiore
// e poi mettere un controllo che dealloca solo se nella VMT ci sta lei.
// oppure mettere una specie di setter al contrario. bisogna capirlo bene.
// ho dovuto fare il cambio tipo all'inizio sennò non lo accettava il puntatore a funzione della VMT.
int Ipc_free(Resource* resource){
    Ipc_desetter(resource);
    return PoolAllocator_releaseBlock(&_ipc_allocator, (Ipc*) resource);
}

void Ipc_desetter(Resource* resource){
    // 1.
    Resource_desetter(resource);
    // 2.
    Ipc* ipc = (Ipc*) resource;
    assert(ipc->waiting_list_read.size == 0 && "Fatal error during IPC free (waiting_list_read not empty). Kernel Panic!");
    assert(ipc->waiting_list_write.size == 0 && "Fatal error during IPC free (waiting_list_write not empty). Kernel Panic!");
    // 3.
    return;
}


// Questa non ha molto senso renderla polimorfica, il costruttore prende in ingresso dati diversi (seppure la struttura è la stessa)
// Poi come faccio a gestire il puntatore a funzione? Non avrebbe senso!
int Ipc_mk(int resource_id, int size_max){
    // 1. Check resource_id if is not valid (anonymous or negative)
    if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
        return DSOS_EINVAL;

    // 2. Check size_max if is not valid (negative)
    if(size_max < 0)
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

// LA WRITE E' OSTINATA E LA READ E' PIGRA

// Nota: la read non è atomica, legge almeno un byte (con 0 si blocca) e ritorna quanti ne ha letti.
int Ipc_read(Descriptor* descriptor, void* buffer, int count){
    // 1. Assert on argument
    assert(descriptor && "");
    assert(buffer && "");
    assert(count>= 0 && "");

    // 2. Cast resource pointer in descriptor to ipc.
    Ipc* ipc = (Ipc*) descriptor->resource;
    assert(ipc && "");

    // 3. In case count == 0 we can return immediately 0 (non essendo arrivati nemmeno alla coda lo stato della risorsa è invariato)
    if(count == 0){
        return 0;
    }

    // 4. If ipc->size == 0 we can't handle the read request, so there is two cases:
    //  a. If DSOS_O_NONBLOCK setted we return DSOS_EAGAIN
    //  b. Else we put the process in the waiting_list_read
    if(ipc->size == 0){
        if(descriptor->flags & DSOS_O_NONBLOCK) return DSOS_EAGAIN;
        // 1. Metto il mio PCB in coda dentro la waiting_list_read
        running->status = Waiting;
        running->syscall_retvalue = DSOS_ERESTARTNOINTR;
        running = (PCB*) List_insert(&(ipc->waiting_list_read), (ipc->waiting_list_read).last, (ListItem*) running);
        assert(running && "ERRORE INSERIMENTO LISTA ATTESA");

        // 2. Prendo il prossimo PCB in stato di ready e lo imposto come ready
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        assert(running && "");
        running->status = Running;
        //3. Passo il controllo alla trap che eseguirà il context switch, internal read non copierà il valore DSOS_ERESTARTNOINTR dentro al PCB
        return DSOS_ERESTARTNOINTR;
    }
    // 5. Exec the read (ipc manage only how many byte we read, the effective data management are implemented by the up level)
    int n_read = ipc->size < count ? ipc->size : count;
    ipc->size -= n_read;

    // 6. Unlock a writer (if there is at least one writer waiting)
    if(ipc->waiting_list_write.size > 0){
        // 6.a. Remove from writer waiting list
        PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_write), (ipc->waiting_list_write).first);
        assert(unlocking && "");
        // 6.b Insert into raedy list and change status
        unlocking->status = Ready;
        unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
        assert(unlocking && "");
    }

    // 7. Unlock a reader (If there is still data and at least one reader waiting)
    if(ipc->size > 0 && ipc->waiting_list_read.size > 0){
        // 7.a. Remove from reader waiting list
        PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_read), (ipc->waiting_list_read).first);
        assert(unlocking && "");
        // 7.b Insert into raedy list and change status
        unlocking->status = Ready;
        unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
        assert(unlocking && "");
    }

    // 8. Return how many byte read
    return n_read;
}

// RICORDA CHE E' ATOMICA O PUO' SCRIVERE O FALLISCE
// Ma cosa accade se count > max_size? -> SE NON BLOCCANTE SCRIVE; SE BLOCCANTE SCRIVE A BLOCCHI (QUINDI SI MANTIENE IN CODA PRATICAMENTE)
// --> Se count > max_size decade l'atomicità della syscall quindi posso scrivere blocchi ad ogni disponibilità (anche 1 byte).
int Ipc_write(Descriptor* descriptor, const void* buffer, int count){
    // 1. Assert on argument
    assert(descriptor && "");
    assert(buffer && "");
    assert(count>= 0 && "");

    // 2. Cast resource pointer in descriptor to ipc.
    Ipc* ipc = (Ipc*) descriptor->resource;
    assert(ipc && "");

    // 3. In case count == 0 we can return immediately 0 (non essendo arrivati nemmeno alla coda lo stato della risorsa è invariato)
    if(count == 0){
        return 0;
    }

    // 4. If DSOS_O_NONBLOCK setted we try to write (best effort, at least 1 byte) and return number of writted byte
    //      if ipc->size == ipc->size_max return DSOS_O_EAGAIN
    if(descriptor->flags & DSOS_O_NONBLOCK){
        if(ipc->size_max - ipc->size == 0) return DSOS_EAGAIN;
    }else{
    // 5. Blocking the writer if count <= ipc->size_max && size_max-size < count or if ipc->size_max - ipc_size == 0
        if(ipc->size_max - ipc->size == 0 || (count <= ipc->size_max && ipc->size_max - ipc->size < count)){
            // 1. Metto il mio PCB in coda dentro la waiting_list_write
            running->status = Waiting;
            running = (PCB*) List_insert(&(ipc->waiting_list_write), (ipc->waiting_list_write).last, (ListItem*) running);
            assert(running && "ERRORE INSERIMENTO LISTA ATTESA");
            running->syscall_retvalue = DSOS_ERESTARTNOINTR;

            // 2. Prendo il prossimo PCB in stato di ready e lo imposto come ready
            running = (PCB*) List_detach(&ready_list, ready_list.first);
            assert(running && "");
            running->status = Running;
            //3. Passo il controllo alla trap che eseguirà il context switch
            return DSOS_ERESTARTNOINTR;            
        }
    }

    // 6. Execute the write
    int n_write = (ipc->size_max - ipc->size) >= count ? count : ipc->size_max - ipc->size;
    ipc->size += n_write;


    // 7. Unlock a reader (If there is at least one reader)
    if(ipc->waiting_list_read.size > 0){
        // 7.a. Remove from reader waiting list
        PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_read), (ipc->waiting_list_read).first);
        assert(unlocking && "");
        // 7.b Insert into raedy list and change status
        unlocking->status = Ready;
        unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
        assert(unlocking && "");
    }

    // 8. Unlock a writer (if there is still available space and at least one writer waiting)
    if(ipc->size_max - ipc->size > 0 && ipc->waiting_list_write.size > 0){
        // 6.a. Remove from writer waiting list
        PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_write), (ipc->waiting_list_write).first);
        assert(unlocking && "");
        // 6.b Insert into ready list and change status
        unlocking->status = Ready;
        unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
        assert(unlocking && "");
    }

    // 9. Return how many byte write
    return n_write;
}

PoolAllocator* Ipc_allocator_getinfo(){return &_ipc_allocator;}