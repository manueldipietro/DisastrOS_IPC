#include "circular_buffer.h"

#include <string.h>
#include <assert.h>

// read_pos e write_pos non sincronizzate, si assume che sia la classe superiore a tracciare il numero di byte liberi.

// SRC: buffer della fifo
// DST: buffer utente
void Circular_buffer_read(const char *src, char* dest, int to_read, int size_max, int* read_pos){
    // TODO: mettere su entrambi assert su non negatività e non nullità
    
    assert(to_read <= size_max && "In Circular_buffer_read try to read more than size_max");
    assert(*read_pos < size_max && "In Circular_buffer_read buffer raed_pos out of bound");
    if((*read_pos+to_read) <= size_max){
        //void *memcpy(void dest[restrict .n], const void src[restrict .n], size_t n);
        memcpy(dest, src+(*read_pos), to_read);
    }
    else{
        int first_copy_length = size_max-(*read_pos);
        memcpy(dest, src+(*read_pos), first_copy_length);
        memcpy(dest+first_copy_length, src, to_read-first_copy_length);
    }
    *read_pos = (*read_pos + to_read)%size_max;
    return;
}

// SRC: buffer utente
// Dest: buffer della pipo
void Circular_buffer_write(const char *src, char* dest, int to_write, int size_max, int* write_pos){
    assert(to_write <= size_max && "In Circular_buffer_write try to write more than size_max");
    assert(*write_pos < size_max && "In Circular_buffer_write buffer write_pos out of bound");
    
    if((*write_pos+to_write) <= size_max){
        memcpy(dest+(*write_pos), src, to_write);
    }
    else{
        int first_copy_length = size_max-(*write_pos); 
        memcpy(dest+(*write_pos), src, first_copy_length);
        memcpy(dest, src+first_copy_length, to_write-first_copy_length);
    }
    *write_pos = (*write_pos + to_write)%size_max;
    return;
}
