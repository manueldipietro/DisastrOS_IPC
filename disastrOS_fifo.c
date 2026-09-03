#include "disastrOS_fifo.h"

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
    // 2. Set attributes
    Fifo_setter(fifo, resource_id, DSOS_RESTYPE_IPCFIFO, Fifo_onopen, Fifo_onclose, Fifo_onclone, Fifo_read, Fifo_write, Fifo_free, DSOS_PIPE_BUF);
    // 3. Return pointer to the fifo
    return fifo;
}

void Fifo_setter(Fifo* fifo, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_onclone_fn onclone_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn, int size_max){
    // 1. call base class setter
    Ipc* ipc = &(fifo->ipc);
    Ipc_setter(ipc, resource_id, resource_type, onopen_fn, onclose_fn, onclone_fn, read_fn, write_fn, free_fn, size_max);
    // 2. Set the open sync attributes and initialize open waiting lists
    fifo->readers_number = 0;
    fifo->writers_number = 0;
    List_init(&fifo->waiting_list_open_reader);
    List_init(&fifo->waiting_list_open_writer);
    // 3. Set the data attributes and the initialize the buffer
    fifo->read_pos = 0;
    fifo->write_pos = 0;
    memset(fifo->buffer, 0, DSOS_PIPE_BUF);
    // 4. Return
    return;
}

int Fifo_free(Resource* resource){
    Fifo_desetter(resource);
    return PoolAllocator_releaseBlock(&_fifo_allocator, (Fifo*) resource);
}

void Fifo_desetter(Resource* resource){
    // 1. Check if there are waiting processes
    Fifo* fifo = (Fifo*) resource;
    assert(fifo->waiting_list_open_reader.size == 0 && "Fatal error during Fifo free (waiting_list_open_reader not empty). Kernel Panic!");
    assert(fifo->waiting_list_open_writer.size == 0 && "Fatal error during Fifo free (waiting_list_open_writer not empty). Kernel Panic!");
    // 2. Call base class desetter
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
    // 1. Assert on arguments
    assert(descriptor && "Fatal error during Fifo onclone (descriptor is NULL). Kernel Panic!");
    assert((descriptor->flags & DSOS_O_ACCMODE) != DSOS_O_RDWR && "Fatal error during Fifo onclone (unsupported access mode). Kernel Panic!");
    // 2. Retrieve Fifo from descriptor
    Fifo* fifo = (Fifo*) descriptor->resource;
    assert(fifo && "Fatal error during Fifo onclone (Fifo is NULL). Kernel Panic!");
    // 3. Increment readers or writers number
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY) fifo->readers_number++;
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY) fifo->writers_number++;
    // 4. Return
    return;
}

int Fifo_onopen(Descriptor* descriptor){
    // 1. Retrieve resource from descriptor
    Fifo* fifo = (Fifo*) descriptor->resource;
    assert(descriptor && "Error on Fifo_onopen (Fifo is NULL). Kernel Panic!");
    // 2. Check access mode, if is equals to DSOS_O_RDWR (unsupported) return EINVAL
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDWR) return DSOS_EINVAL;
    // 3. Case access mode is RDONLY
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY){
        // 3.a. Case where descriptor is non blocking or there are writers opened or waiting for open
        if(fifo->writers_number > 0 || fifo->waiting_list_open_writer.size > 0 || descriptor->flags & DSOS_O_NONBLOCK){
            // 3.a.i. Unlock all writers waiting for open (we will use intermediate_data for track if writers_number was yet incremented)
            while(fifo->waiting_list_open_writer.size > 0){
                PCB* unlocking = (PCB*) List_detach(&(fifo->waiting_list_open_writer), (fifo->waiting_list_open_writer).first);
                assert(unlocking && "Error on Fifo_onopen (unlocking PCB is NULL after detach). Kernel Panic!");
                unlocking->status = Ready;
                unlocking->syscall_intermediate_data = 1;
                fifo->writers_number++;
                unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
                assert(unlocking && "Error on Fifo_onopen (unlocking PCB is NULL after ListInsert). Kernel Panic!");
            }
            if(running->syscall_intermediate_data == 0){
                ++fifo->readers_number;
            }else{running->syscall_intermediate_data = 0;}
            return DSOS_SUCCESS;
        }
        // 3.b Case where descriptor is blocking and there aren't writers opened or waiting for open
        running->status = Waiting;
        running->syscall_retvalue = DSOS_ERESTARTNOINTR;
        running = (PCB*) List_insert(&(fifo->waiting_list_open_reader), (fifo->waiting_list_open_reader).last, (ListItem*) running);
        assert(running && "Error on Fifo_onopen (Error while insert reader in waiting list). Kernel Panic!");
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        assert(running && "Error on Fifo_onopen (Error while detach next running process). Kernel Panic!");
        running->status = Running;
        return DSOS_ERESTARTNOINTR;
    }
    // 4. Case access mode is WRONLY
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY){
        // 4.a. Case where there are no readers opened or waiting for open
        if(fifo->readers_number > 0 || fifo->waiting_list_open_reader.size > 0){
            // 4.a.i. Unlock all readers waiting for open (we will use intermediate_data for track if readers_number was yet incremented)
            while(fifo->waiting_list_open_reader.size > 0){
                PCB* unlocking = (PCB*) List_detach(&(fifo->waiting_list_open_reader), (fifo->waiting_list_open_reader).first);
                assert(unlocking && "Error on Fifo_onopen (unlocking PCB is NULL after detach). Kernel Panic!");
                unlocking->status = Ready;
                unlocking->syscall_intermediate_data = 1;
                fifo->readers_number++;
                unlocking = (PCB*) List_insert(&ready_list, ready_list.last, (ListItem*) unlocking);
                assert(unlocking && "Error on Fifo_onopen (unlocking PCB is NULL after ListInsert). Kernel Panic!");
            }
            if(running->syscall_intermediate_data == 0){
                ++fifo->writers_number;
            }else{running->syscall_intermediate_data = 0;}
            return DSOS_SUCCESS;
        }
        // 4.b.i. Case where descriptor is non blocking and there aren't reader return error DSOS_ENXIO
        if(descriptor->flags & DSOS_O_NONBLOCK) return DSOS_ENXIO;
        // 4.b.ii. Case where descriptor is blocking and there aren't readers opened or waiting for open
        running->status = Waiting;
        running = (PCB*) List_insert(&(fifo->waiting_list_open_writer), (fifo->waiting_list_open_writer).last, (ListItem*) running);
        assert(running && "Error on Fifo_onopen (Errore while insert writer in waiting list). Kernel Panic!");
        running->syscall_retvalue = DSOS_ERESTARTNOINTR;
        running = (PCB*) List_detach(&ready_list, ready_list.first);
        assert(running && "Error on Fifo_onopen (Error while detach next running process). Kernel Panic!");
        running->status = Running;
        //3. Passo il controllo alla trap che eseguirà il context switch
        return DSOS_ERESTARTNOINTR;
    }
    assert(!"Unexpected error. Kernel Panic!");
}

void Fifo_onclose(Descriptor* descriptor){
    // 1. Assert on access mode (RDWR not supproted)
    assert((descriptor->flags & DSOS_O_ACCMODE) != DSOS_O_RDWR && "Error on Fifo_onclose (DSOS_O_RDWR mode unsupported). Kernel Panic!");
    // 2. Retrieve resource and assert on it
    Fifo* fifo = (Fifo*) descriptor->resource;
    assert(fifo && "Error on Fifo_onclose (Fifo is NULL). Kernel Panic!");
    // 3. If Fifo opened in RDONLY mode decrements readers number and if readers_number==0 unlock all waiting writers
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY){
        --fifo->readers_number;
        if(fifo->readers_number == 0){
            Ipc* ipc = &(fifo->ipc);
            while(ipc->waiting_list_write.size)
                Ipc_unlock_first_writer(ipc);
        }
    }
    // 4. If Fifo opened in WRONLY mode decrements writers number and if writers_number==0 unlock all waiting readers
    if((descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY){
        --fifo->writers_number;
        if(fifo->writers_number == 0){
            Ipc* ipc = &(fifo->ipc);
            while(ipc->waiting_list_read.size)
                Ipc_unlock_first_reader(ipc);
        }
    }
    // 5. Return
    return;
}

int Fifo_read(Descriptor* descriptor, void* buffer, int count){
    // 1. Validate arguments
    assert(descriptor && "Fatal error during Fifo_read (descriptor is NULL). Kernel Panic!");
    assert(buffer && "Fatal error during Fifo_read (buffer is NULL). Kernel Panic!");
    assert(count>= 0 && "Fatal error during Fifo_read (negative count). Kernel Panic!");
    // 2. Retrive fifo from descriptor
    Fifo* fifo = (Fifo*) descriptor->resource;
    assert(fifo && "Error on Fifo_read (Fifo is NULL). Kernel Panic!");
    // 3. If ipc.size == 0 and writers == 0 return immediately 0
    if(fifo->ipc.size == 0 && fifo->writers_number == 0) return 0;// In realtà fifo->ipc rompe l'incapsulamento servirebbe una funzione ausiliaria --> NON VALE LA PENA
    // 4. Call Ipc read
    int to_read = Ipc_read(descriptor, buffer, count);
    if(to_read <= 0) return to_read;
    // 5. Read from buffer
    Circular_buffer_read(fifo->buffer, (char*) buffer, to_read, DSOS_PIPE_BUF, &fifo->read_pos);
    // 6. Return readed bytes
    return to_read;
}

int Fifo_write(Descriptor* descriptor, const void* buffer, int count){
    // 1. Validate arguments
    assert(descriptor && "Fatal error during Fifo_write (descriptor is NULL). Kernel Panic!");
    assert(buffer && "Fatal error during Fifo_write (buffer is NULL). Kernel Panic!");
    assert(count>= 0 && "Fatal error during Fifo_write (negative count). Kernel Panic!");
    // 2. Retrive fifo from descriptor
    Fifo* fifo = (Fifo*) descriptor->resource;
    assert(fifo && "Error on Fifo_write (Fifo is NULL). Kernel Panic!");
    // 3. If readers == 0 return immediately EOF
    if(fifo->readers_number == 0) return DSOS_EPIPE;
    // 4. Call Ipc write
    int to_write = Ipc_write(descriptor, buffer, count);
    if(to_write <= 0) return to_write;
    // 5. Write to buffer and return
    Circular_buffer_write((const char*) buffer, fifo->buffer, to_write, DSOS_PIPE_BUF, &fifo->write_pos);
    // 6. return writed bytes
    return to_write;
}

PoolAllocator* Fifo_allocator_getinfo(){return &_fifo_allocator;}