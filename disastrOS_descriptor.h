#pragma once
#include "disastrOS_resource.h"
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "pool_allocator.h"

struct DescriptorPtr;

typedef struct Descriptor{
  ListItem list;
  PCB* pcb;
  Resource* resource;
  int fd;
  struct DescriptorPtr* ptr;
  int flags;
} Descriptor;

typedef struct DescriptorPtr{
  ListItem list;
  Descriptor* descriptor;
} DescriptorPtr;

// Memory management
void Descriptor_init();
Descriptor* Descriptor_alloc(int fd, Resource* res, PCB* pcb, int flags);
int Descriptor_free(Descriptor* d);
DescriptorPtr* DescriptorPtr_alloc(Descriptor* descriptor);
int DescriptorPtr_free(DescriptorPtr* d);

// Function for resource class
int Descriptor_mk(Descriptor** descriptor, Resource* resource, int flags);
void Descriptor_destroy(Descriptor* descriptor);

// Auxiliary Function
Descriptor*  DescriptorList_byFd(ListHead* l, int fd);
DescriptorPtr*  DescriptorPtrList_byDesc(ListHead* l, Descriptor* descriptor);

// Debug/tester support functions
void DescriptorList_print(ListHead* l);
PoolAllocator* Descriptor_allocator_getinfo();
void DescriptorPtrList_print(ListHead* l);
PoolAllocator* DescriptorPtr_allocator_getinfo();