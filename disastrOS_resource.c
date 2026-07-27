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
    assert(! result);
    return;
}

// Alloc, rimane e non effettua nessun controllo sul parametro passato, è pensata per uso interno sicuro.
// Se si riuscisse a scomporre tra allocatore e inizializzatore si potrebbe gestire l'inizializzazione
// senza ripetere troppo codice
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

  // 3. Fills the VMT
  (resource->VMT).open = Resource_open;
  (resource->VMT).read = Resource_read;
  (resource->VMT).write = Resource_write;
  (resource->VMT).close = Resource_close;
  (resource->VMT).unlink = Resource_unlink;

  // 4. Initialize the resource list
  List_init(&resource->descriptors_ptrs);
  
  // 5. Return the pointer to the resource
  return resource;
}

/**QUESTA L'HO RIDEFINITA IO, PRIMA DI TOGLIERE LA ALLOC TOCCA CAPIRE DOVE VIENE CHIAMATA! */
int Resource_mk(int resource_id){
  // 0. Controlla resource_id
  // 1. Controlla se la risorsa già esiste
  // 2. Chiama l'allocatore, e ritorna ENOMEM se ritorna NULL.
  // 3. Ritorna
  return 0;
}

int Resource_open(int resource_id, int flags){
  //DEBUG DA CANCELLARE
  printf("ENTRO DENTRO LA OPEN!!!!\n");

  // 1. Check arguments: if resource_id is valid (anonymous or out of bound)
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

  printf("Sopravvivo!\n");

  // 3. Query for resource
  Resource* res = ResourceList_byId(&resources_list, resource_id);

  // 4. We distinguish the behavior based on whether the resource exists or not
  //    and on the DSOS_O_CREAT and DSOS_O_EXCL (That means: create resource but
  //    fail if exists yet) flags:
  //    a. Resource NOT EXIST
  //      a.1. DSOS_O_CREAT OFF: error! Return DSOS_ENOENT;
  //      a.2. DSOS_O_CREAT  ON: create resource and open (DSOS_O_EXCL not important)
  //    b. Resource EXIST (we need to consider only if DSOS_O_CREAT ON else we open resource)
  //      b.1. DSOS_O_CREAT ON and DSOS_O_EXCL ON: error! Return DSOS_EXIST
  //      b.2. DSOS_O_CREAT ON and DSOS_O_EXCL OFF: open resource (Default behavior)
  //      b.3. DSOS_O_CREAT OFF: open resource (Default behavior)
  // a
  if(res == NULL){
    // a.1
    if(!(flags & DSOS_O_CREAT)) return DSOS_ENOENT;
    // a.2
    //INSERIRE CREAZIONE RISORSA + CONTROLLO BUON FINE Altrimenti tornare errore ottenuto
    res = Resource_alloc(resource_id);
    if(!res) return DSOS_ENOMEM;
    List_insert(&resources_list, resources_list.last, (ListItem*) res);
  }
  // b
  else
    // b.1
    if((flags & DSOS_O_CREAT) && (flags & DSOS_O_EXCL)) return DSOS_EXIST;
  // b.2 b.3 --> Default behavior

  printf("Risorsa: %p\n", res);

  // TODO: riguardare codice apertura descrittori per ridefinire errori in standard "POSIX"
  // TODO: compattare dentro una funzione Descriptor_mk;
  // 5. Create descriptor for the resource
  Descriptor* des = Descriptor_alloc(running->last_fd, res, running);
  if(!des){
    return -1; //Errore allocazione descrittore;
  }
  running->last_fd++; // we increment the fd value for the next call
  DescriptorPtr* desptr=DescriptorPtr_alloc(des);
  List_insert(&running->descriptors, running->descriptors.last, (ListItem*) des);

  // 6. Add to the resource, in the descriptor ptr list, apointer to the newly
  des->ptr=desptr;
  List_insert(&res->descriptors_ptrs, res->descriptors_ptrs.last, (ListItem*) desptr);

  return des->fd;
}

/**
  IDEA: USARE QUESTE DUE FUNZIONI COME CONTROLLO DI BASE SU PARAMETRI (TRANNE BUFFER E FD)?
*/
int Resource_read(int fd, void* buffer, int count){
  printf("Chiamata Resource Read\n");
  //printf("SYSCALL_READ_ERROR: Operation not supported on resources of undefined type\n");
  return 0;
}
int Resource_write(int fd, const void* buffer, int count){
  printf("Chiamata Resource Read\n");
  //printf("SYSCALL_WRITE_ERROR: Operation not supported on resources of undefined type\n");
  return 0;
}

int Resource_close(int fd){
  printf("Called Resource_close with fd: %d\n", fd);
  return 0;
}

int Resource_unlink(int resource_id){
  printf("Called Resource unlink resource_id: %d\n", resource_id);
  return 0;
  /*
  // 0. I controlli di apertura possono essere copiati dalla open
  Resource* res;
  // 1. We set unlink field to 1
  res->unlink = 1;
  // 2. We call destroyer (that effectively destroy only if there isn't fd in list)
  Resource_destroy(resource_id);
  return 0;*/
}

int Resource_destroy(int resource_id){
  // NOTA CHE DEVE CONTROLLARE CHE UNLINK SIA UGUALE A 1
  
  // 0. I controlli di apertura possono essere copiati dalla open
  
  // 1. Query for the resource
  
  // 2. If resource doesn't exist return error
  
  // 3. Ensure the resource is not used by any process and destroy it, else exit.
    // IF (NOT IN USE) --> DESTROY
  
  return 0;
}

int Resource_free(Resource* r) {
  assert(r->descriptors_ptrs.first==0);
  assert(r->descriptors_ptrs.last==0);
  return PoolAllocator_releaseBlock(&_resources_allocator, r);
}

Resource* ResourceList_byId(ResourceList* l, int id) {
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
