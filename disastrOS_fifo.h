#pragma once
#include "disastrOS_ipc.h"

// Fifo class
typedef struct Fifo{
    Ipc ipc;
    // Open sync section
    int readers_number;
    int writers_number;
    ListHead waiting_list_open_reader;
    ListHead waiting_list_open_writer;
    // Data section
    int read_pos;
    int write_pos;
    char buffer[DSOS_PIPE_BUF];
}Fifo;

// Memory management
void Fifo_init();
Fifo* Fifo_alloc(int id);
int Fifo_free(Resource* resource);

// Posix interface
int Fifo_mk(int resource_id);
int Pipe_mk(int pipefd[2]);

// Ovveride function
int Fifo_onopen(Descriptor* descriptor);
void Fifo_onclose(Descriptor* descriptor);
void Fifo_onclone(Descriptor* descriptor);
int Fifo_read(Descriptor* descriptor, void* buffer, int count);
int Fifo_write(Descriptor* descriptor, const void* buffer, int count);

// Auxiliary function
void Fifo_setter(Fifo* fifo, int resource_id, int resource_type, disastros_onopen_fn onopen_fn, disastros_onclose_fn onclose_fn, disastros_onclone_fn onclone_fn, disastros_read_fn read_fn, disastros_write_fn write_fn, disastros_free_fn free_fn, int size_max);
void Fifo_desetter(Resource* resource);

// Debug/tester support functions
PoolAllocator* Fifo_allocator_getinfo();