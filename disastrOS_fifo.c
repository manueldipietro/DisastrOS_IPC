#include "disastrOS_fifo.h"
#include "disastrOS_ipc.h"

#include "circular_buffer.h"
#include "disastrOS_globals.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>


#define FIFO_SIZE sizeof(Fifo)
#define FIFO_MEMSIZE (sizeof(Fifo)+sizeof(int))
#define FIFO_BUFFER_SIZE MAX_NUM_FIFOS*FIFO_MEMSIZE

static char _fifo_buffer[FIFO_BUFFER_SIZE];
static PoolAllocator _fifo_allocator;

void Fifo_init(){
    int result = PoolAllocator_init(&_fifo_allocator, FIFO_SIZE, MAX_NUM_FIFOS, _fifo_buffer, FIFO_BUFFER_SIZE);
    assert(!result && "Error during FIFO Pool Allocator init. Kernel Panic!");
    return;
}

Fifo* Fifo_alloc(int resource_id){
    // 0. Validate resource_id
    assert(resource_id >= 0 && "Fatal error during Fifo alloc (resource_id negative). Kernel Panic!");
    // 1. Allocate Fifo, if allocation goes wrong return NULL
    Fifo* fifo = (Fifo*) PoolAllocator_getBlock(&_fifo_allocator);
    if(!fifo) return NULL;
    Fifo_setter(fifo, resource_id, DSOS_RESTYPE_IPCFIFO, Fifo_onopen, Fifo_onclose, Fifo_onclone, Fifo_read, Fifo_write, Fifo_free, PIPE_BUF);
    return fifo;
}

void Fifo_setter(Fifo* fifo, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_onclone_fn onclone_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn, int size_max){
    // 1. 
    Ipc* ipc = &(fifo->ipc);
    Ipc_setter(ipc, resource_id, resource_type, onopen_fn, onclose_fn, onclone_fn, read_fn, write_fn, free_fn, size_max);
    
    // 2.
    fifo->readers_number = 0;
    fifo->writers_number = 0;
    List_init(&fifo->waiting_list_open_reader);
    List_init(&fifo->waiting_list_open_writer);

    // 3.
    fifo->read_pos = 0;
    fifo->write_pos = 0;
    memset(fifo->buffer, 0, PIPE_BUF);
    
    // 4. Return
    return;
}

int Fifo_free(Resource* resource){
    Fifo_desetter(resource);
    return PoolAllocator_releaseBlock(&_fifo_allocator, (Fifo*) resource);
}

void Fifo_desetter(Resource* resource){
    Ipc_desetter(resource);
    return;
}

int Fifo_mk(int resource_id){
    // 1. Check resource_id if is not valid (anonymous or negative)
    if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
        return DSOS_EINVAL;
    // 2. Check if resource (fifo) does not existing
    Fifo* fifo = (Fifo*) ResourceList_byId(&resources_list, resource_id);
    if(fifo) return DSOS_EEXIST;
    // 3. Alloc resource and return DSOS_ENOMEM if out of memory, insert new fifo in resources_list
    fifo = Fifo_alloc(resource_id);
    if(!fifo) return DSOS_ENOMEM;
    fifo = (Fifo*) List_insert(&resources_list, resources_list.last, (ListItem*) fifo);
    assert(fifo && "Fatal error during fifo mk (list_insert). Kernel Panic!");
    // 4. Return DSOS_SUCCESS
    return DSOS_SUCCESS;
}

int Pipe_mk(int pipefd[2]){
    // 1. Alloc anonymous Pipe
    Fifo* pipe = Fifo_alloc(dsos_last_anon_resource_id);
    if(!pipe) return DSOS_ENOMEM;
    dsos_last_anon_resource_id++;

    // 2. Alloc first descriptor
    Descriptor* descriptor_rd;
    int ret_val = Descriptor_mk(&descriptor_rd, (Resource*) pipe, DSOS_O_RDONLY);
    if(ret_val != DSOS_SUCCESS){
        Resource_destroy((Resource*) pipe);
        return ret_val;
    }
    pipefd[DSOS_PIPE_RD] = descriptor_rd->fd;
    pipe->readers_number = 1;

    //3. Alloc second descriptor, if goes wrong roll back
    Descriptor* descriptor_wr;
    ret_val = Descriptor_mk(&descriptor_wr, (Resource*) pipe, DSOS_O_WRONLY);
    if(ret_val != DSOS_SUCCESS){
        Descriptor_destroy(descriptor_wr);
        Resource_destroy((Resource*) pipe);
        return ret_val;
    }
    pipefd[DSOS_PIPE_WR] = descriptor_wr->fd;
    pipe->writers_number = 1;

    // 4. All ok
    return DSOS_SUCCESS;
}

void Fifo_onclone(Descriptor* descriptor){
    assert(descriptor && "");
    assert((descriptor->flags & DSOS_O_ACCMODE) != DSOS_O_RDWR && "");

    Fifo* fifo = (Fifo*) descriptor->resource;
    assert(fifo && "");
    
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY) fifo->readers_number++;
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY) fifo->writers_number++;
    return;
}

int Fifo_onopen(Descriptor* descriptor){
    Fifo* fifo = (Fifo*) descriptor->resource;
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDWR) return DSOS_EINVAL;

    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY){
        if(fifo->writers_number > 0 || fifo->waiting_list_open_writer.size > 0 || descriptor->flags & DSOS_O_NONBLOCK){
            while(fifo->waiting_list_open_writer.size > 0){
                // 6.a. Remove from writer waiting list
                PCB* unlocking = (PCB*) List_detach(&(fifo->waiting_list_open_writer), (fifo->waiting_list_open_writer).first);
                assert(unlocking && "");
                // 6.b Insert into ready list and change status
                unlocking->status = Ready;
                unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
                unlocking->syscall_intermediate_data = 1;
                fifo->writers_number++;
                assert(unlocking && "");
            }
            if(running->syscall_intermediate_data == 0){
                ++fifo->readers_number;
            }else{running->syscall_intermediate_data = 0;}
            return DSOS_SUCCESS;
        }
        // 1. Metto il mio PCB in coda dentro la waiting_list_open_read
        running->status = Waiting;
        running->syscall_retvalue = DSOS_ERESTARTNOINTR;
        running = (PCB*) List_insert(&(fifo->waiting_list_open_reader), (fifo->waiting_list_open_reader).last, (ListItem*) running);
        assert(running && "ERRORE INSERIMENTO LISTA ATTESA");
        // 2. Prendo il prossimo PCB in stato di ready e lo imposto come ready
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        assert(running && "");
        running->status = Running;
        //3. Passo il controllo alla trap che eseguirà il context switch, internal read non copierà il valore DSOS_ERESTARTNOINTR dentro al PCB
        return DSOS_ERESTARTNOINTR;
    }


    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY){
        if(fifo->readers_number > 0 || fifo->waiting_list_open_reader.size > 0){
            while(fifo->waiting_list_open_reader.size > 0){
                // 6.a. Remove from writer waiting list
                PCB* unlocking = (PCB*) List_detach(&(fifo->waiting_list_open_reader), (fifo->waiting_list_open_reader).first);
                assert(unlocking && "");
                // 6.b Insert into ready list and change status
                unlocking->status = Ready;
                unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
                unlocking->syscall_intermediate_data = 1;
                fifo->readers_number++;
                assert(unlocking && "");
            }
            if(running->syscall_intermediate_data == 0){
                ++fifo->writers_number;
            }else{running->syscall_intermediate_data = 0;}
            return DSOS_SUCCESS;
        }
        if(descriptor->flags & DSOS_O_NONBLOCK) return DSOS_ENXIO;
        running->status = Waiting;
        running = (PCB*) List_insert(&(fifo->waiting_list_open_writer), (fifo->waiting_list_open_writer).last, (ListItem*) running);
        assert(running && "ERRORE INSERIMENTO LISTA ATTESA");
        running->syscall_retvalue = DSOS_ERESTARTNOINTR;
        // 2. Prendo il prossimo PCB in stato di ready e lo imposto come ready
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        assert(running && "");
        running->status = Running;
        //3. Passo il controllo alla trap che eseguirà il context switch
        return DSOS_ERESTARTNOINTR;
    }
    assert(!"Unexpected error. Kernel Panic!");
}

void Fifo_onclose(Descriptor* descriptor){
    assert((descriptor->flags & DSOS_O_ACCMODE) != DSOS_O_RDWR && "");
    Fifo* fifo = (Fifo*) descriptor->resource;
    //In realtà la ACCMODE RDWR NON E' CONSENTITA, BISOGNA TROVARE UN MODO PER NON FARLA AVVENIRE
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY){
        --fifo->readers_number;
        if(fifo->readers_number == 0){
            Ipc* ipc = &(fifo->ipc);
            while(ipc->waiting_list_write.size){
                // 6.a. Remove from writer waiting list
                PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_write), (ipc->waiting_list_write).first);
                assert(unlocking && "");
                // 6.b Insert into ready list and change status
                unlocking->status = Ready;
                unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
                assert(unlocking && "");
            }
        }
    }
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY){
        --fifo->writers_number;
        if(fifo->writers_number == 0){
            Ipc* ipc = &(fifo->ipc);
            while(ipc->waiting_list_read.size){
                // 7.a. Remove from reader waiting list
                PCB* unlocking = (PCB*) List_detach(&(ipc->waiting_list_read), (ipc->waiting_list_read).first);
                assert(unlocking && "");
                // 7.b Insert into raedy list and change status
                unlocking->status = Ready;
                unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
                assert(unlocking && "");
            }
        }
    }
    return;
}

int Fifo_read(Descriptor* descriptor, void* buffer, int count){
        // 0. Retrive fifo from descriptor
        Fifo* fifo = (Fifo*) descriptor->resource;
        // 1. If ipc.size == 0 and writers == 0 return immediately 0
        if(fifo->ipc.size == 0 && fifo->writers_number == 0) return 0;// In realtà fifo->ipc rompe l'incapsulamento servirebbe una funzione ausiliaria --> NON VALE LA PENA
        // 2. Call Ipc read
        int to_read = Ipc_read(descriptor, buffer, count);
        if(to_read <= 0) return to_read;
        // 3. Read from buffer
        Circular_buffer_read(fifo->buffer, (char*) buffer, to_read, PIPE_BUF, &fifo->read_pos);
        //4. Return readed bytes
        return to_read;
}

int Fifo_write(Descriptor* descriptor, const void* buffer, int count){
        // 0. Retrive fifo from descriptor
        Fifo* fifo = (Fifo*) descriptor->resource;
        // 1. If readers == 0 return immediately EOF
        if(fifo->readers_number == 0) return DSOS_EPIPE;
        // 2. Call Ipc write
        int to_write = Ipc_write(descriptor, buffer, count);
        if(to_write <= 0) return to_write;
        // 3. Write to buffer and return
        Circular_buffer_write((const char*) buffer, fifo->buffer, to_write, PIPE_BUF, &fifo->write_pos);
        return to_write;
}

PoolAllocator* Fifo_allocator_getinfo(){return &_fifo_allocator;}