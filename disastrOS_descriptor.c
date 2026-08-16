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
#define MAX_NUM_DESCRIPTORS_PTRS  MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE*MAX_NUM_PROCESSES
#define DESCRIPTORPTR_BUFFER_SIZE MAX_NUM_DESCRIPTORS_PTRS*DESCRIPTORPTR_MEMSIZE

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

Descriptor* Descriptor_alloc(int fd, Resource* res, PCB* pcb, int flags) {
  Descriptor* d=(Descriptor*)PoolAllocator_getBlock(&_descriptor_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->fd=fd;
  d->resource=res;
  d->pcb=pcb;
  d->flags = flags;
  return d;
}

int Descriptor_free(Descriptor* d) {
  return PoolAllocator_releaseBlock(&_descriptor_allocator, d);
}

int Descriptor_mk(Descriptor** descriptor, Resource* resource, int flags){
  // 1. Check if the maximum number of descriptors ptrs for the resource has been reached
  if(resource->descriptors_ptrs.size >= MAX_NUM_DESCRIPTORS_PTRS_PER_RESOURCE)
    return DSOS_ENFILE;

  // 2. Check if the maximum number of descriptors for the file has been reached 
  if(running->descriptors.size >= MAX_NUM_DESCRIPTORS_PER_PROCESS)
    return DSOS_EMFILE;
  
  // 3. Alloc new descriptor and list insert into processes' descriptor list
  *descriptor = Descriptor_alloc(running->last_fd, resource, running, flags);
  assert((*descriptor) && "Fatal error during descriptor mk (null descriptor). Kernel Panic!");
  (*descriptor) = (Descriptor*) List_insert(&running->descriptors, running->descriptors.last, (ListItem*) (*descriptor));
  assert((*descriptor) && "Fatal error during descriptor mk (list_insert descriptor). Kernel Panic!");
  
  // 4. Increment the fd value for the next call
  running->last_fd++;
  
  // 5. Alloc new descriptor pointer and list insert it into resource's descriptorPtr list
  DescriptorPtr* descriptor_ptr=DescriptorPtr_alloc(*descriptor);
  assert(descriptor_ptr && "Fatal error during descriptor mk (null descriptor_ptr). Kernel Panic!");
  (*descriptor)->ptr=descriptor_ptr;
  descriptor_ptr = (DescriptorPtr*) List_insert(&resource->descriptors_ptrs, resource->descriptors_ptrs.last, (ListItem*) descriptor_ptr);
  assert(descriptor_ptr && "Fatal error during descriptor mk (list_insert descriptor_ptr). Kernel Panic!");

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
  
  return;
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

DescriptorPtr*  DescriptorPtrList_byDesc(ListHead* l, Descriptor* descriptor){
  ListItem* aux=l->first;
  while(aux){
    DescriptorPtr* dp=(DescriptorPtr*)aux;
    if (dp->descriptor==descriptor) return dp;
    aux=aux->next;
  }
  return 0;
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

// Only for test purpose
PoolAllocator* Descriptor_allocator_getinfo(){return &_descriptor_allocator;}
PoolAllocator* DescriptorPtr_allocator_getinfo(){return &_descriptor_ptr_allocator;}