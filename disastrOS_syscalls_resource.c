#include <assert.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"

// Correggere con internal open
void internal_openResource(){
  printf("CHIAMATA SYSCALL OPEN\n");
  // QUI DENTRO CI RESTA L'APERTURA DELLA RISORA SPECIFICA.
  // E L'EVENTUALE CHIAMATA A OPEN SPECIALIZATA SE SERVE (MA NON CREDO SERVA)
  
  // Facciamo Dispatch argomenti e open().
  
  int id = running->syscall_args[0];
  int flags = running->syscall_args[1];
  // TO DO: andare a coreggere argomenti
  //        sulle call

  int ret = Resource_open(id, flags);
  printf("La OPEN HA RITORNATO: %d\n", ret);
  running->syscall_retvalue = ret;

  return;
  /*
  //1 get from the PCB the resource id of the resource to open
  int id=running->syscall_args[0];
  int type=running->syscall_args[1];
  int open_mode=running->syscall_args[2];

  Resource* res=ResourceList_byId(&resources_list, id);
  //2 if the resource is opened in CREATE mode, create the resource
  //  and return an error if the resource is already existing
  // otherwise fetch the resource from the system list, and if you don't find it
  // throw an error
  //printf ("CREATING id %d, type: %d, open mode %d\n", id, type, open_mode);
  if (open_mode&DSOS_CREATE){
    if (res) {
      running->syscall_retvalue=DSOS_ERESOURCECREATE;
      return;
    }
    res=Resource_alloc(id, type);
    List_insert(&resources_list, resources_list.last, (ListItem*) res);
  }

  // at this point we should have the resource, if not something was wrong
  if (! res || res->type!=type) {
     running->syscall_retvalue=DSOS_ERESOURCEOPEN;
     return;
  }
  
  if (open_mode&DSOS_EXCL && res->descriptors_ptrs.size){
     running->syscall_retvalue=DSOS_ERESOURCENOEXCL;
     return;
  }

  
  //5 create the descriptor for the resource in this process, and add it to
  //  the process descriptor list. Assign to the resource a new fd
  Descriptor* des=Descriptor_alloc(running->last_fd, res, running);
  if (! des){
     running->syscall_retvalue=DSOS_ERESOURCENOFD;
     return;
  }
  running->last_fd++; // we increment the fd value for the next call
  DescriptorPtr* desptr=DescriptorPtr_alloc(des);
  List_insert(&running->descriptors, running->descriptors.last, (ListItem*) des);
  
  //6 add to the resource, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  des->ptr=desptr;
  List_insert(&res->descriptors_ptrs, res->descriptors_ptrs.last, (ListItem*) desptr);

  // return the FD of the new descriptor to the process
  running->syscall_retvalue = des->fd;
  */
}

void internal_read(){
  // 0. Retrieve the arguments of syscall
  //int fd = running->syscall_args[0];
  //void* buffer = (void*) running->syscall_args[1];
  //int count = running->syscall_args[2];

  // 1. From fd retrieve the resource, and check validity
  // NOTA: qui ci va anche validazione del FILEDESCRIPTOR
  // Altrimenti si rischiano inconsistente
  // Perchè non scrivere una funzione che lo fa automaticamente?
  //Resource* resource;

  // 2. Using VMT for call read
  //int ret = ((resource->VMT).read)(fd, buffer, count);

  // 3. Return value
  //running->syscall_retvalue = ret;
}

void internal_write(){
  // STESSA STRUTTURA DELLA READ
  return;
}

void internal_closeResource(){
  printf("CHIAMATA SYSCALL CLOSE\n");
  /*
  //1 retrieve the fd of the resource to close
  int fd=running->syscall_args[0];


  Descriptor* des=DescriptorList_byFd(&running->descriptors, fd);
  //2 if the fd is not in the the process, we return an error
  if (! des){
    running->syscall_retvalue=DSOS_ERESOURCECLOSE;
    return;
  }

  //3 we remove the descriptor from the process list
  des = (Descriptor*) List_detach(&running->descriptors, (ListItem*) des);
  assert(des);

  Resource* res=des->resource;

  // we remove the descriptor pointer from the resource list
  DescriptorPtr* desptr=(DescriptorPtr*) List_detach(&res->descriptors_ptrs, (ListItem*)(des->ptr));
  assert(desptr);
  Descriptor_free(des);
  DescriptorPtr_free(desptr);
  running->syscall_retvalue=0;
  */
}

void internal_destroyResource(){
  printf("CHIAMATA SYSCALL DESTOY\n");

  /*
  int id=running->syscall_args[0];

  // find the resource in with the id
  Resource* res=ResourceList_byId(&resources_list, id);
  if (! res){
    running->syscall_retvalue=DSOS_ERESOURCECLOSE;
    return;
  }

  // ensure the resource is not used by any process
  if(res->descriptors_ptrs.size){
    running->syscall_retvalue=DSOS_ERESOURCEINUSE;
    return;
  }

  res=(Resource*) List_detach(&resources_list, (ListItem*) res);
  assert(res);
  Resource_free(res);
  running->syscall_retvalue=0;
  */
}
