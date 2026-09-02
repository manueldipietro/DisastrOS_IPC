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

void Descriptor_init();

Descriptor* Descriptor_alloc(int fd, Resource* res, PCB* pcb, int flags);
int Descriptor_free(Descriptor* d);

int Descriptor_mk(Descriptor** descriptor, Resource* resource, int flags);
void Descriptor_destroy(Descriptor* descriptor);

Descriptor*  DescriptorList_byFd(ListHead* l, int fd);
void DescriptorList_print(ListHead* l);

DescriptorPtr* DescriptorPtr_alloc(Descriptor* descriptor);
int DescriptorPtr_free(DescriptorPtr* d);
void DescriptorPtrList_print(ListHead* l);

DescriptorPtr*  DescriptorPtrList_byDesc(ListHead* l, Descriptor* descriptor);

// Only for test purpose
PoolAllocator* Descriptor_allocator_getinfo();
PoolAllocator* DescriptorPtr_allocator_getinfo();