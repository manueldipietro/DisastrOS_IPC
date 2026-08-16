#include <assert.h>
#include <stdio.h>

#include "disastrOS_globals.h"
#include "disastrOS_constants.h"

#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"

#include "pool_allocator.h"
#include "linked_list.h"

#define RESOURCE_SIZE sizeof(Resource)
#define RESOURCE_MEMSIZE (sizeof(Resource)+sizeof(int))
#define RESOURCE_BUFFER_SIZE MAX_NUM_RESOURCES*RESOURCE_MEMSIZE

static char _resources_buffer[RESOURCE_BUFFER_SIZE];
static PoolAllocator _resources_allocator;

void Resource_init(){
    int result=PoolAllocator_init(& _resources_allocator, RESOURCE_SIZE, MAX_NUM_RESOURCES, _resources_buffer, RESOURCE_BUFFER_SIZE);
    assert(!result && "Error during Resourc Poll Alloator init! Kernel Panic!");
    return;
}

Resource* Resource_alloc(int resource_id){
  // 1. Allocate resource, if allocation goes wrong return NULL
  Resource* resource = (Resource*) PoolAllocator_getBlock(&_resources_allocator);
  if(!resource)
    return NULL;

  // 2. Fills the fields of the resource
  (resource->list).prev = (resource->list).next = 0;
  resource->id = resource_id;
  resource->type = DSOS_RESTYPE_UNDEFIN;
  resource->unlinked = (resource_id >= DSOS_ANON_RES_STARTID ? 1 : 0);

  // 3. Fills the VMT, read and write are NULL because resource will be a virtual class,
  //    but for test purpose we mantain the constructor and destructor
  (resource->VMT).read = NULL;
  (resource->VMT).write = NULL;
  (resource->VMT).free = Resource_free;

  // 4. Initialize the descriptors_ptrs list
  List_init(&resource->descriptors_ptrs);
  
  // 5. Return the pointer to the resource
  return resource;
}

int Resource_mk(int resource_id){
  // 1. Check resource_id if is not valid (anonymous or negative)
  if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
    return DSOS_EINVAL;

  // 2. Check if resource does not existing
  Resource* resource = ResourceList_byId(&resources_list, resource_id);
  if(resource) return DSOS_EEXIST;

  // 3. Alloc resource and return DSOS_ENOMEM if out of memory, insert new resource on resources_list
  resource = Resource_alloc(resource_id);
  if(!resource) return DSOS_ENOMEM;
  resource = (Resource*) List_insert(&resources_list, resources_list.last, (ListItem*) resource);
  assert(resource && "Fatal error during resource mk (list_insert). Kernel Panic!");
  
  // 4. Return DSOS_SUCCESS
  return DSOS_SUCCESS;
}

int Resource_open(int resource_id, int flags){
  // 1. Check resource_id if is not valid (anonymous or negative)
  if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
    return DSOS_EINVAL;

  // 2. Check flags, and return DSOS_EINVAL in the following cases:
  //    a. Unsupported flags
  int supported_flags = DSOS_O_ACCMODE | DSOS_O_CREAT | DSOS_O_EXCL | DSOS_O_NONBLOCK;
  if(flags & (~supported_flags)) return DSOS_EINVAL;
  //    b. DSOS_O_RDONLY, DSOS_O_WRONLY and DSOS_O_RDWR setted
  if( (flags&DSOS_O_ACCMODE)!=DSOS_O_RDONLY && (flags&DSOS_O_ACCMODE)!=DSOS_O_WRONLY && (flags&DSOS_O_ACCMODE)!=DSOS_O_RDWR) return DSOS_EINVAL;
  //    c. DSOS_O_EXCL specified without DSOS_O_CREAT 
  if(!(flags & DSOS_O_CREAT) && (flags & DSOS_O_EXCL)) return DSOS_EINVAL;

  // 3. Query for resource
  Resource* resource = ResourceList_byId(&resources_list, resource_id);

  // 4. If resource non-existing we distinguish two case:
  //    a Flags DSOS_O_CREAT setted (with or without DSOS_EXCL): create resource and continue
  //        Note: in this case we can have error ENOMEM because we can't alloc the resource
  //    b Falgs DSOS_O_CREAT not setted: return DSOS_ENOENT error
  if(resource == NULL){
    if(flags & DSOS_O_CREAT){
      resource = Resource_alloc(resource_id);
      if(!resource) return DSOS_ENOMEM;
      resource = (Resource*) List_insert(&resources_list, resources_list.last, (ListItem*) resource);
      assert(resource && "Fatal error during open (list_insert resource). Kernel Panic!");
    }else{
      return DSOS_ENOENT;
    }
  }
  // 5. Else if resource exist (not null) and DSOS_O_CREAT and DSOS_O_EXCL (exclusive) both setted we return DSOS_EEXIST error
  else{
    if((flags & DSOS_O_CREAT) && (flags & DSOS_O_EXCL)) return DSOS_EEXIST;
  }

  // 6. Allocate descriptors. Note: Descriptor_mk can generate DSOS_ENFILE, DSOS_EMFILE
  Descriptor* descriptor;
  int ret_val = Descriptor_mk(&descriptor, resource, flags);
  if(ret_val != DSOS_SUCCESS) return ret_val;
  
  // 7. Return file descriptor
  return descriptor->fd;
}

int Resource_read(int fd, void* buffer, int count){
  // 1. Query for the file descriptor and check if is valid (exist and has correct flags)
  Descriptor* descriptor = DescriptorList_byFd(&running->descriptors, fd);
  if(!descriptor) return DSOS_EBADFD;

  // 2. Check access mode
  if( (descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_WRONLY) return DSOS_EBADFD;

  // 3. Check the validity of the buffer and count
  if(!buffer || count<0) return DSOS_EINVAL;

  // 4. Retrive resource pointer and validate it
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during resource read (resource null pointer). Kernel Panic!");

  // 5. Use VMT for call virtual method
  disastros_read_fn virtual_read = resource->VMT.read;
  if(!virtual_read) return DSOS_ENOSYS;
  int ret_value = virtual_read(fd, buffer, count);

  // 6. Return value returned from virtual method
  return ret_value;
}

int Resource_write(int fd, const void* buffer, int count){
  // 1. Query for the file descriptor and check if is valid (exist and has correct flags)
  Descriptor* descriptor = DescriptorList_byFd(&running->descriptors, fd);
  if(!descriptor) return DSOS_EBADFD;

  // 2. Check access mode
  if( (descriptor->flags & DSOS_O_ACCMODE) == DSOS_O_RDONLY) return DSOS_EBADFD;

  // 3. Check the validity of the buffer and count
  if(!buffer || count<0) return DSOS_EINVAL;

  // 4. Retrive resource pointer and validate it
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during resource write (resource null pointer). Kernel Panic!");

  // 5. Use VMT for call virtual method
  disastros_write_fn virtual_write = resource->VMT.write;
  if(!virtual_write) return DSOS_ENOSYS;
  int ret_value = virtual_write(fd, buffer, count);

  // 6. Return value returned from virtual method
  return ret_value;
}

int Resource_close(int fd){
  // 1. Query for the file descriptor and check if is valid
  if(fd < 0) return DSOS_EBADFD;
  Descriptor* descriptor = DescriptorList_byFd(&running->descriptors, fd);
  if(!descriptor) return DSOS_EBADFD;
  
  // 2. Retrive resource pointer and validate it
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during resource close (resource null pointer). Kernel Panic!");

  // 3. Destroy the file descriptor
  Descriptor_destroy(descriptor);

  // 4. Try to dealloc resources
  Resource_destroy(resource);
     
  // 5. Return DSOS_SUCCESS
  return DSOS_SUCCESS;
}

int Resource_unlink(int resource_id){
  // 1. Check resource_id if is valid (anonymous or out of bound)
  if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
    return DSOS_EINVAL;

  // 2. Query for the resource
  Resource* resource = ResourceList_byId(&resources_list, resource_id);
  if(resource == NULL) return DSOS_ENOENT;

  // 3. Set unlinked field to 1
  resource->unlinked = 1;

  // 4. Remove the resource from resources_list
  resource = (Resource*) List_detach(&resources_list, (ListItem*) resource);
  assert(resource && "Fatal error during List detach. Kernel Panic!");

  // 5. Try to destroy during the close
  Resource_destroy(resource);

  // 6. Return DSOS_SUCCESS
  return DSOS_SUCCESS;
}

//Questa in realtà non va all'utente, l'utente non deve poter distruggere la risorsa, ma solo farne l'unlink.
//Quindi qui unificare il gestore della distruzione (delete va qui).
void Resource_destroy(Resource* resource){
  // 1. Check if there is file descriptor and in case return and check pointer for avoiding SEGFAULT
  if(!resource || !resource->unlinked || (resource->descriptors_ptrs).size)
    return;

  // 2. Dealloc the resource
  disastros_resource_free_fn virtual_free = resource->VMT.free;
  assert(virtual_free && "Fatal error during Resource free (allocator non implemented. Kernel Panic!)");
  int free_sucess = virtual_free(resource);
  assert(!free_sucess && "Fatal error during Resource free. Kernel Panic!");

  // 3. Return
  return;
}

int Resource_free(Resource* r) {
  assert(r->descriptors_ptrs.first==0);
  assert(r->descriptors_ptrs.last==0);
  return PoolAllocator_releaseBlock(&_resources_allocator, r);
}

Resource* ResourceList_byId(ResourceList* l, int id){
  ListItem* aux=l->first;
  while(aux){
    Resource* r=(Resource*)aux;
    if (r->id==id)
      return r;
    aux=aux->next;
  }
  return 0;
}

// Only for debug purpose
void Resource_print(Resource* r) {
  printf("id: %d, type:%d, pids:", r->id, r->type);
  DescriptorPtrList_print(&r->descriptors_ptrs);
}

void ResourceList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("{\n");
  while(aux){
    Resource* r=(Resource*)aux;
    printf("\t");
    Resource_print(r);
    if(aux->next)
      printf(",");
    printf("\n");
    aux=aux->next;
  }
  printf("}\n");
}

// Only for test purpose
PoolAllocator* Resource_allocator_getinfo(){return &_resources_allocator;}