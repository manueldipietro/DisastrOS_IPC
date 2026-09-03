#include "disastrOS_ipc.h"

#include <assert.h>
#include <stdio.h>

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

Ipc* Ipc_alloc(int resource_id, int size_max){
    // 0. Validate size_max and resource_id
    assert(resource_id >=0 && "Fatal error during IPC alloc (resource_id negative). Kernel Panic!");
    assert(size_max >=0 && "Fatal error during IPC alloc (size_max negative). Kernel Panic!");
    // 1. Allocate IPC, if allocation goes wrong return NULL
    Ipc* ipc = (Ipc*) PoolAllocator_getBlock(&_ipc_allocator);
    if(!ipc)
        return NULL;
    // 2. Set attributes
    Ipc_setter(ipc, resource_id, DSOS_RESTYPE_IPCBASE, NULL, NULL, NULL, Ipc_read, Ipc_write, Ipc_free, size_max);
    // 3. Return pointer to the ipc
    return ipc;
}

void Ipc_setter(Ipc* ipc, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn,  disastros_onclone_fn onclone_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn, int size_max){
    // 1. Call base class setter
    Resource* resource = &ipc->resource;
    Resource_setter(resource, resource_id, resource_type, onopen_fn, onclose_fn, onclone_fn, read_fn, write_fn, free_fn);
    // 2. Set the size and size_max attribute
    ipc->size = 0;
    ipc->size_max = size_max;    
    // 3. Initialize the waiting lists
    List_init(&ipc->waiting_list_read);
    List_init(&ipc->waiting_list_write);
    return;
}

int Ipc_free(Resource* resource){
    Ipc_desetter(resource);
    return PoolAllocator_releaseBlock(&_ipc_allocator, (Ipc*) resource);
}

void Ipc_desetter(Resource* resource){
    // 1. Check if there are waiting processes
    Ipc* ipc = (Ipc*) resource;
    assert(ipc->waiting_list_read.size == 0 && "Fatal error during IPC free (waiting_list_read not empty). Kernel Panic!");
    assert(ipc->waiting_list_write.size == 0 && "Fatal error during IPC free (waiting_list_write not empty). Kernel Panic!");
    // 2. Call base class desetter
    Resource_desetter(resource);
    return;
}

int Ipc_mk(int resource_id, int size_max){
    // 1. Check resource_id if is not valid (anonymous or negative)
    if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
        return DSOS_EINVAL;
    // 2. Check size_max if is not valid (negative)
    if(size_max < 0)
        return DSOS_EINVAL;
    // 3. Check if ipc exist
    Ipc* ipc = (Ipc*) ResourceList_byId(&resources_list, resource_id);
    if(ipc) return DSOS_EEXIST;
    // 4. Alloc resource and return DSOS_ENOMEM if out of memory, insert new ipc on resources_list
    ipc = Ipc_alloc(resource_id, size_max);
    if(!ipc) return DSOS_ENOMEM;
    ipc = (Ipc*) List_insert(&resources_list, resources_list.last, (ListItem*) ipc);
    assert(ipc && "Fatal error during ipc mk (list_insert). Kernel Panic!");
    // 5. Return DSOS_SUCCESS
    return DSOS_SUCCESS;
}

int Ipc_lock_reader(Ipc* ipc){
    // 1. Setting data on running PCB (status and ret_value)
    running->status = Waiting;
    running->syscall_retvalue = DSOS_ERESTARTNOINTR;
    // 2. Insert running PCB in waiting list
    running = (PCB*) List_insert(&(ipc->waiting_list_read), (ipc->waiting_list_read).last, (ListItem*) running);
    assert(running && "Fatal error during IPC lock reader (running PCB is NULL). Kernel Panic!");
    // 3. Detach from ready list first ready list PCB and put in running status
    running = (PCB*) List_detach(&ready_list, ready_list.first);
    assert(running && "Fatal error during IPC lock reader (list detach return NULL pointer). Kernel Panic!");
    running->status = Running;
    // 4. Return DSOS_ERESTARTNOINTR (internal code for restart syscall)
    return DSOS_ERESTARTNOINTR;
}

int Ipc_lock_writer(Ipc* ipc){
    // 1. Setting data on running PCB (status and ret_value)
    running->status = Waiting;
    running->syscall_retvalue = DSOS_ERESTARTNOINTR;
    // 2. Insert running PCB in waiting list
    running = (PCB*) List_insert(&(ipc->waiting_list_write), (ipc->waiting_list_write).last, (ListItem*) running);
    assert(running && "Fatal error during IPC lock writer (running PCB is NULL). Kernel Panic!");
    // 3. Detach from ready list first ready list PCB and put in running status
    running = (PCB*) List_detach(&ready_list, ready_list.first);
    assert(running && "Fatal error during IPC lock writer (list detach return NULL pointer). Kernel Panic!");
    running->status = Running;
    // 4. Return DSOS_ERESTARTNOINTR (internal code for restart syscall)
    return DSOS_ERESTARTNOINTR;            
}

void Ipc_unlock_first_reader(Ipc* ipc){
    // 1. Validate waiting list read size
    assert(ipc->waiting_list_read.size > 0 && "Fatal error during IPC unlock first reader (waiting list read is empty). Kernel Panic!");    
    // 2. Detach first waiting process PCB from waiting list read
    PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_read), (ipc->waiting_list_read).first);
    assert(unlocking && "Fatal error during IPC unlock first reader (list detach return NULL pointer). Kernel Panic!");
    // 3. Insert unlocked PCB into ready list
    unlocking->status = Ready;
    unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
    assert(unlocking && "Fatal error during IPC unlock first reader (list insert return NULL pointer). Kernel Panic!");
    // 4. Return to syscall
    return;
}

void Ipc_unlock_first_writer(Ipc* ipc){
    // 1. Validate waiting list write size
    assert(ipc->waiting_list_write.size > 0 && "Fatal error during IPC unlock first writer (waiting list read is empty). Kernel Panic!");    
    // 2. Detach first waiting process PCB from waiting list write
    PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_write), (ipc->waiting_list_write).first);
    assert(unlocking && "Fatal error during IPC unlock first writer (list detach return NULL pointer). Kernel Panic!");
    // 3. Insert unlocked PCB into ready list
    unlocking->status = Ready;
    unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
    assert(unlocking && "Fatal error during IPC unlock first writer (list insert return NULL pointer). Kernel Panic!");
    // 4. Return to syscall
    return;
}

int Ipc_read(Descriptor* descriptor, void* buffer, int count){
    // 1. Validate arguments
    assert(descriptor && "Fatal error during IPC read (descriptor is NULL). Kernel Panic!");
    assert(buffer && "Fatal error during IPC read (buffer is NULL). Kernel Panic!");
    assert(count>= 0 && "Fatal error during IPC read (negative count). Kernel Panic!");
    // 2. Cast resource pointer in descriptor to ipc.
    Ipc* ipc = (Ipc*) descriptor->resource;
    assert(ipc && "Fatal error during IPC read (can't retrieve resource from IPC). Kernel Panic!");
    // 3. In case count == 0 we can return immediately 0
    if(count == 0) return 0;
    // 4. If ipc->size == 0 we can't handle the read request, so there are two cases:
    if(ipc->size == 0){
        //  a. If DSOS_O_NONBLOCK setted we return DSOS_EAGAIN
        if(descriptor->flags & DSOS_O_NONBLOCK) return DSOS_EAGAIN;
        //  b. Else we put the process in the waiting_list_read
        return Ipc_lock_reader(ipc);
    }
    // 5. Exec the read (ipc manage only how many byte we read, the effective data management are implemented by the up level)
    int n_read = ipc->size < count ? ipc->size : count;
    ipc->size -= n_read;
    // 6. Unlock a writer (if there is at least one writer waiting)
    if(ipc->waiting_list_write.size > 0)
        Ipc_unlock_first_writer(ipc);
    // 7. Unlock a reader (If there is still data and at least one reader waiting)
    if(ipc->size > 0 && ipc->waiting_list_read.size > 0)
        Ipc_unlock_first_reader(ipc);
    // 8. Return how many byte read
    return n_read;
}

int Ipc_write(Descriptor* descriptor, const void* buffer, int count){
    // 1. Validate arguments
    assert(descriptor && "Fatal error during IPC read (descriptor is NULL). Kernel Panic!");
    assert(buffer && "Fatal error during IPC read (buffer is NULL). Kernel Panic!");
    assert(count>= 0 && "Fatal error during IPC read (negative count). Kernel Panic!");
    // 2. Cast resource pointer in descriptor to ipc.
    Ipc* ipc = (Ipc*) descriptor->resource;
    assert(ipc && "Fatal error during IPC write (can't retrieve resource from IPC). Kernel Panic!");
    // 3. In case count == 0 we can return immediately 0
    if(count == 0) return 0;
    // 4. If DSOS_O_NONBLOCK setted we try to write (best effort, at least 1 byte) and return number of writted byte, if ipc->size == ipc->size_max return DSOS_O_EAGAIN
    if(descriptor->flags & DSOS_O_NONBLOCK){
        if(ipc->size_max - ipc->size == 0) return DSOS_EAGAIN;
    }
    // 5. Blocking the writer if count <= ipc->size_max && size_max-size < count or if ipc->size_max - ipc_size == 0
    else{
        if(ipc->size_max - ipc->size == 0 || (count <= ipc->size_max && ipc->size_max - ipc->size < count)){
            return Ipc_lock_writer(ipc);
        }
    }
    // 6. Execute the write
    int n_write = (ipc->size_max - ipc->size) >= count ? count : ipc->size_max - ipc->size;
    ipc->size += n_write;
    // 7. Unlock a reader (If there is at least one reader)
    if(ipc->waiting_list_read.size > 0)
        Ipc_unlock_first_reader(ipc);
    // 8. Unlock a writer (if there is still available space and at least one writer waiting)
    if(ipc->size_max - ipc->size > 0 && ipc->waiting_list_write.size > 0)
        Ipc_unlock_first_writer(ipc);
    // 9. Return how many byte write
    return n_write;
}

PoolAllocator* Ipc_allocator_getinfo(){return &_ipc_allocator;}