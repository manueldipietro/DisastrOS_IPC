#include "circular_buffer.h"

#include <string.h>
#include <assert.h>

void Circular_buffer_read(const char *src, char* dest, int to_read, int size_max, int* read_pos){
    // 0. Validate arguments before circular buffer execution
    assert(src != NULL && "In Circular_buffer_read src argument is NULL. Kernel Panic!");
    assert(dest != NULL && "In Circular_buffer_read dst argument is NULL. Kernel Panic!");    
    assert(read_pos != NULL && "In Circular_buffer_read read_pos argument is NULL. Kernel Panic!");   
    assert(to_read >= 0 && "In Circular_buffer_read to_read argument is negative. Kernel Panic!");
    assert(size_max >= 0 && "In Circular_buffer_read size_max argument is negative. Kernel Panic!");
    assert(to_read <= size_max && "In Circular_buffer_read try to read more than size_max. Kernel Panic!");
    assert(*read_pos >= 0 && *read_pos < size_max && "In Circular_buffer_read read_pos argument is invalid. Kernel Panic!");    
    // 1. Read and update read_pos
    if((*read_pos+to_read) <= size_max){
        memcpy(dest, src+(*read_pos), to_read);
    }
    else{
        int first_copy_length = size_max-(*read_pos);
        memcpy(dest, src+(*read_pos), first_copy_length);
        memcpy(dest+first_copy_length, src, to_read-first_copy_length);
    }
    *read_pos = (*read_pos + to_read)%size_max;
    // 2. Return
    return;
}

void Circular_buffer_write(const char *src, char* dest, int to_write, int size_max, int* write_pos){
    // 0. Validate arguments before circular buffer execution
    assert(src != NULL && "In Circular_buffer_write src argument is NULL. Kernel Panic!");
    assert(dest != NULL && "In Circular_buffer_write dst argument is NULL. Kernel Panic!");
    assert(write_pos != NULL && "In Circular_buffer_write write_pos argument is NULL. Kernel Panic!");
    assert(to_write >= 0 && "In Circular_buffer_write to_write argument is negative. Kernel Panic!");
    assert(size_max >= 0 && "In Circular_buffer_write size_max argument is negative. Kernel Panic!");
    assert(to_write <= size_max && "In Circular_buffer_write try to write more than size_max");
    assert(*write_pos >= 0 && *write_pos < size_max && "In Circular_buffer_write write_pos argument is invalid. Kernel Panic!");    
    // 1. Write and update write_pos
    if((*write_pos+to_write) <= size_max){
        memcpy(dest+(*write_pos), src, to_write);
    }
    else{
        int first_copy_length = size_max-(*write_pos); 
        memcpy(dest+(*write_pos), src, first_copy_length);
        memcpy(dest, src+first_copy_length, to_write-first_copy_length);
    }
    *write_pos = (*write_pos + to_write)%size_max;
    // 2. Return
    return;
}
