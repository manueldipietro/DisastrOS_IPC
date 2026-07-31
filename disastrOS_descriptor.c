#include <assert.h>
#include <stdio.h>

#include "disastrOS_globals.h"
#include "disastrOS_constants.h"

#include "disastrOS_descriptor.h"
#include "disastrOS_resource.h"

#include "pool_allocator.h"
#include "linked_list.h"

#define DESCRIPTOR_SIZE sizeof(Descriptor)
#define DESCRIPTOR_MEMSIZE (sizeof(Descriptor)+sizeof(int))
#define MAX_NUM_DESCRIPTORS (MAX_NUM_DESCRIPTORS_PER_PROCESS*MAX_NUM_PROCESSES)
#define DESCRIPTOR_BUFFER_SIZE MAX_NUM_DESCRIPTORS*DESCRIPTOR_MEMSIZE

#define DESCRIPTORPTR_SIZE sizeof(DescriptorPtr)
#define DESCRIPTORPTR_MEMSIZE (sizeof(DescriptorPtr)+sizeof(int))
#define DESCRIPTORPTR_BUFFER_SIZE MAX_NUM_DESCRIPTORS*DESCRIPTORPTR_MEMSIZE

static char _descriptor_buffer[DESCRIPTOR_BUFFER_SIZE];
static PoolAllocator _descriptor_allocator;

static char _descriptor_ptr_buffer[DESCRIPTORPTR_BUFFER_SIZE];
static PoolAllocator _descriptor_ptr_allocator;

void Descriptor_init(){
  int result=PoolAllocator_init(& _descriptor_allocator,
				DESCRIPTOR_SIZE,
				MAX_NUM_PROCESSES,
				_descriptor_buffer,
				DESCRIPTOR_BUFFER_SIZE);
  assert(! result);

  result=PoolAllocator_init(& _descriptor_ptr_allocator,
			    DESCRIPTORPTR_SIZE,
			    MAX_NUM_PROCESSES,
			    _descriptor_ptr_buffer,
			    DESCRIPTORPTR_BUFFER_SIZE);
  assert(! result);
}

Descriptor* Descriptor_alloc(int fd, Resource* res, PCB* pcb) {
  Descriptor* d=(Descriptor*)PoolAllocator_getBlock(&_descriptor_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->fd=fd;
  d->resource=res;
  d->pcb=pcb;
  return d;
}

int Descriptor_free(Descriptor* d) {
  return PoolAllocator_releaseBlock(&_descriptor_allocator, d);
}

int Descriptor_mk(Descriptor** descriptor, Resource* resource){
  // 1. Check if the maximum number of descriptors for the resource has been reached
  if(resource->descriptors_ptrs.size >= MAX_NUM_RESOURCES_PER_PROCESS)
    return DSOS_ENFILE;

  // 2. Check if the maximum number of descriptors for the file has been reached 
  if(running->descriptors.size >= MAX_NUM_DESCRIPTORS_PER_PROCESS)
    return DSOS_EMFILE;
  
  // 3. Alloc new descriptor and list insert into processes' descriptor list
  *descriptor = Descriptor_alloc(running->last_fd, resource, running);
  if(!(*descriptor)) return DSOS_ENOMEM;
  List_insert(&running->descriptors, running->descriptors.last, (ListItem*) (*descriptor));
  
  // 4. Increment the fd value for the next call
  running->last_fd++;
  
  // 5. Alloc new descriptor pointer and list insert it into resource's descriptorPtr list
  //    and manage case when there is error with allocation of descriptor pointer
  DescriptorPtr* descriptor_ptr=DescriptorPtr_alloc(*descriptor);
  // TODO: QUI VA GESTITO IL CASO IN CUI CI SIA UN ERRORE, STACCANDO IL DESCRITTORE DALLA LISTA, DEALLOCANDOLO E RITORNANDO L'ERRORE (ENOMEM)
  (*descriptor)->ptr=descriptor_ptr;
  List_insert(&resource->descriptors_ptrs, resource->descriptors_ptrs.last, (ListItem*) descriptor_ptr);

  // 6. Return success
  return DSOS_SUCCESS;
}

void Descriptor_destroy(Descriptor* descriptor){
  // 1. Retrieve resource pointer and validate it 
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during descriptor destroy (resource null pointer). Kernel Panic!");
  
  // 2. List detach the descriptor from process' list
  descriptor = (Descriptor*) List_detach(&running->descriptors, (ListItem*) descriptor);
  assert(descriptor && "Fatal error during List detach (Descriptor). Kernel Panic!");
  
  // 3. List detach the descriptor pointer from resource's list
  DescriptorPtr* descriptor_pointer = (DescriptorPtr*) List_detach(&resource->descriptors_ptrs, (ListItem*)(descriptor->ptr));
  assert(descriptor_pointer && "Fatal error during List detach (Descriptor Pointer). Kernel Panic!");
  
  //4. Dealloc descriptor and descriptor pointer
  Descriptor_free(descriptor);
  DescriptorPtr_free(descriptor_pointer);
}


Descriptor*  DescriptorList_byFd(ListHead* l, int fd){
  ListItem* aux=l->first;
  while(aux){
    Descriptor* d=(Descriptor*)aux;
    if (d->fd==fd)
      return d;
    aux=aux->next;
  }
  return 0;
}


DescriptorPtr* DescriptorPtr_alloc(Descriptor* descriptor) {
  DescriptorPtr* d=PoolAllocator_getBlock(&_descriptor_ptr_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->descriptor=descriptor;
  return d;
}

int DescriptorPtr_free(DescriptorPtr* d){
  return PoolAllocator_releaseBlock(&_descriptor_ptr_allocator, d);
}

void DescriptorList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    Descriptor* d=(Descriptor*)aux;
    printf("(fd: %d, rid:%d)",
	   d->fd,
	   d->resource->id);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}


void DescriptorPtrList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    DescriptorPtr* d=(DescriptorPtr*)aux;
    printf("(pid: %d, fd: %d, rid:%d)",
	   	d->descriptor->pcb->pid,
      d->descriptor->fd,
	    d->descriptor->resource->id);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}
