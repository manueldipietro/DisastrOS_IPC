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
  // 1. Allocate Resource, if allocation goes wrong return NULL
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

  // 4. Initialize the resource list
  List_init(&resource->descriptors_ptrs);
  
  // 5. Return the pointer to the resource
  return resource;
}

//Pensato per far creare una risorsa all'utente, non accetta id anonimi.
int Resource_mk(int resource_id){
  // 1. Controlla resource_id
  if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
    return DSOS_EINVAL;

  // 2. Controlla se la risorsa già esiste
  Resource* resource = ResourceList_byId(&resources_list, resource_id);
  if(resource) return DSOS_EEXIST;

  // 3. Alloca la risorsa e ritorna ENOMEM in caso di fallimento.
  resource = Resource_alloc(resource_id);
  if(!resource) return DSOS_ENOMEM;
  List_insert(&resources_list, resources_list.last, (ListItem*) resource);
  //Qui tocca inserire l'assert per l'errore nell'inserimento nella lista.
  // 4. Ritorna 0 in caso di successo
  return DSOS_SUCCESS;
}

int Resource_open(int resource_id, int flags){
  // 1. Check resource_id if is valid (anonymous or out of bound)
  if(resource_id < 0 || resource_id >= DSOS_ANON_RES_STARTID)
    return DSOS_EINVAL;

  // TODO: Bisogna modificare in modo da assumere il comportamento di default come read only quando non si passa il flags di lettura
  // 2. Check flags, and return DSOS_EINVAL in the following cases:
  //    a. Sono stati impostati flags non supportati
  int supported_flags = DSOS_O_RDONLY | DSOS_O_WRONLY | DSOS_O_RDWR | DSOS_O_CREAT | DSOS_O_EXCL | DSOS_O_NONBLOCK;
  if(flags & (~supported_flags)) return DSOS_EINVAL;
  //    b. Sia Read che Write sono impostati a 0
  if(!(flags & (DSOS_O_RDONLY | DSOS_O_WRONLY | DSOS_O_RDWR))) return DSOS_EINVAL;
  //    c. DSOS_O_CREAT OFF && DSOS_O_EXECL ON
  if(!(flags & DSOS_O_CREAT) && (flags & DSOS_O_EXCL)) return DSOS_EINVAL;

  // 3. Query for resource
  Resource* resource = ResourceList_byId(&resources_list, resource_id);

  // 4. We distinguish the behavior based on whether the resource exists or not
  //    and on the DSOS_O_CREAT and DSOS_O_EXCL (That means: create resource but
  //    fail if exists yet) flags:
  //    a. Resource NOT EXIST
  //      a.1. DSOS_O_CREAT OFF: error! Return DSOS_ENOENT;
  //      a.2. DSOS_O_CREAT  ON: create resource and open (DSOS_O_EXCL not important)
  //    b. Resource EXIST (we need to consider only if DSOS_O_CREAT ON else we open resource)
  //      b.1. DSOS_O_CREAT ON and DSOS_O_EXCL ON: error! Return DSOS_EEXIST
  //      b.2. DSOS_O_CREAT ON and DSOS_O_EXCL OFF: open resource (Default behavior)
  //      b.3. DSOS_O_CREAT OFF: open resource (Default behavior)
  // a
  if(resource == NULL){
    // a.1
    if(!(flags & DSOS_O_CREAT)) return DSOS_ENOENT;
    // a.2
    resource = Resource_alloc(resource_id);
    if(!resource) return DSOS_ENOMEM;
    List_insert(&resources_list, resources_list.last, (ListItem*) resource);
  }
  // b
  else
    // b.1
    if((flags & DSOS_O_CREAT) && (flags & DSOS_O_EXCL)) return DSOS_EEXIST;
  // b.2 b.3 --> Default behavior

  Descriptor* descriptor;
  int ret_val = Descriptor_mk(&descriptor, resource);
  if(ret_val != DSOS_SUCCESS) return ret_val;

  return descriptor->fd;
}

/**
  IDEA: USARE QUESTE DUE FUNZIONI COME CONTROLLO DI BASE SU PARAMETRI (TRANNE BUFFER E FD)?
*/
int Resource_read(int fd, void* buffer, int count){
  // 1. Query for the file descriptor and check if is valid (exist and has correct flags)
  Descriptor* descriptor = DescriptorList_byFd(&running->descriptors, fd);
  if(!descriptor) return DSOS_EBADFD;
  // TODO: controllo flag

  // 2. Check the validity of the buffer and count
  if(!buffer || count<=0) return DSOS_EINVAL;

  // 3. Retrive resource pointer and validate it
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during resource read (resource null pointer). Kernel Panic!");

  // 4. Use VMT for call virtual method
  disastros_read_fn virtual_read = resource->VMT.read;
  if(!virtual_read) return DSOS_ENOSYS;
  int ret_value = virtual_read(fd, buffer, count);

  // 5. Return value returned from virtual method
  return ret_value;
}

int Resource_write(int fd, const void* buffer, int count){
  // 1. Query for the file descriptor and check if is valid (exist and has correct flags)
  Descriptor* descriptor = DescriptorList_byFd(&running->descriptors, fd);
  if(!descriptor) return DSOS_EBADFD;
  // TODO: controllo flag

  // 2. Check the validity of the buffer and count
  if(!buffer || count<=0) return DSOS_EINVAL;

  // 3. Retrive resource pointer and validate it
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during resource write (resource null pointer). Kernel Panic!");

  // 4. Use VMT for call virtual method
  disastros_write_fn virtual_write = resource->VMT.write;
  //if(!virtual_write) return DSOS_ENOSYS;
  int ret_value = virtual_write(fd, buffer, count);

  // 5. Return value returned from virtual method
  return ret_value;
}

int Resource_close(int fd){
  // 1. Query for the file descriptor and check if is valid
  Descriptor* descriptor = DescriptorList_byFd(&running->descriptors, fd);
  if(!descriptor) return DSOS_EBADFD;
  
  // 2. Retrive resource pointer and validate it
  Resource* resource = descriptor->resource;
  assert(descriptor->resource && "Fatal error during resource close (resource null pointer). Kernel Panic!");

  // 3. Destroy the file descriptor
  Descriptor_destroy(descriptor);

  // 4. Try to dealloc resources
  Resource_destroy(resource);
     
  // 5. Return Sucess
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
  Resource_destroy(resource); // Va resa polimorfica

  // 6. Return success
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

//DA CORREGGERE PER SUPPORTARE LE RISORSE ANONIME (O forse no se usata da funzioni di sistema?? COntrollare!!)
// RAGGIONAMENTO: essendo questa usata solo dalle funzioni interne del kernel,
// e visto che magari potrebbe essere necessario andare a recupare la risorsa anonima ad esempio nei test
// non la modifico in alcun modo.
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



/**
 * FUNZIONE DI DEBUG
 */
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
