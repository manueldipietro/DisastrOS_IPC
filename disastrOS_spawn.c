#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_descriptor.h"

// Creates a new instance of the running_process and puts it in the ready list, returns the pid of the child
// It starts a function in the form of void f();
void internal_spawn(){
  static PCB* new_pcb;
  new_pcb=PCB_alloc();
  if (!new_pcb) {
    running->syscall_retvalue=DSOS_ESPAWN;
    return;
  } 
  new_pcb->status=Ready;

  // sets the parent of the newly created process to the running process
  new_pcb->parent=running;
  
  // adds a pointer to the new process to the children list of running
  PCBPtr* new_pcb_ptr=PCBPtr_alloc(new_pcb);
  assert(new_pcb_ptr);
  List_insert(&running->children, running->children.last, (ListItem*) new_pcb_ptr);

  //adds the new process to the ready queue
  List_insert(&ready_list, ready_list.last, (ListItem*) new_pcb);

  //sets the retvalue for the caller to the new pid
  running->syscall_retvalue=new_pcb->pid;

  getcontext(&new_pcb->cpu_state);
  new_pcb->cpu_state.uc_stack.ss_sp = new_pcb->stack;
  new_pcb->cpu_state.uc_stack.ss_size = STACK_SIZE;
  new_pcb->cpu_state.uc_stack.ss_flags = 0;
  sigemptyset(&new_pcb->cpu_state.uc_sigmask);
  new_pcb->cpu_state.uc_link = &main_context;
  void (*new_function) (void*)= (void(*)(void*))  running->syscall_args[0];
  makecontext(&new_pcb->cpu_state, (void(*)())  new_function, 1, (void*)running->syscall_args[1]);
}

void internal_spawn_withfd(){
  static PCB* new_pcb;
  new_pcb=PCB_alloc();
  if (!new_pcb) {
    running->syscall_retvalue=DSOS_ESPAWN;
    return;
  } 

  // Section added to handle descriptor duplication
  Descriptor* old_descriptor = (Descriptor*) running->descriptors.first;
  while(old_descriptor){
    // To maintain compatibility with the existing spawn implementation, errors regarding descriptor allocation are not handled.
    // 1. Allocate new_descriptor
    Descriptor* new_descriptor = Descriptor_alloc(old_descriptor->fd, old_descriptor->resource, new_pcb, old_descriptor->flags);
    assert(new_descriptor && "Fatal error during descriptor internal_dup (null descriptor). Kernel Panic!");
    new_descriptor = (Descriptor*) List_insert(&new_pcb->descriptors, new_pcb->descriptors.last, (ListItem*) new_descriptor);
    assert(new_descriptor && "Fatal error during during descriptor internal_dup (list_insert descriptor). Kernel Panic!");
    // 2. Allocate new descriptorPtr
    Resource* resource = new_descriptor->resource;
    DescriptorPtr* new_descriptor_ptr=DescriptorPtr_alloc(new_descriptor);
    assert(new_descriptor_ptr && "Fatal error during during descriptor internal_dup (null descriptor_ptr). Kernel Panic!");
    new_descriptor->ptr=new_descriptor_ptr;
    new_descriptor_ptr = (DescriptorPtr*) List_insert(&resource->descriptors_ptrs, resource->descriptors_ptrs.last, (ListItem*) new_descriptor_ptr);
    assert(new_descriptor_ptr && "Fatal error during during descriptor internal_dup (list_insert descriptor_ptr). Kernel Panic!");
    // 3. Call the specific onclone function of the resource type
    if(resource->VMT.onclone != NULL) resource->VMT.onclone(new_descriptor);
    // 4. Move the list item forward
    old_descriptor = (Descriptor*) old_descriptor->list.next;
  }
  new_pcb->last_fd = running->last_fd;  // This could be a problem
  // End of the added section

  new_pcb->status=Ready;

  // sets the parent of the newly created process to the running process
  new_pcb->parent=running;
  
  // adds a pointer to the new process to the children list of running
  PCBPtr* new_pcb_ptr=PCBPtr_alloc(new_pcb);
  assert(new_pcb_ptr);
  List_insert(&running->children, running->children.last, (ListItem*) new_pcb_ptr);

  //adds the new process to the ready queue
  List_insert(&ready_list, ready_list.last, (ListItem*) new_pcb);

  //sets the retvalue for the caller to the new pid
  running->syscall_retvalue=new_pcb->pid;

  getcontext(&new_pcb->cpu_state);
  new_pcb->cpu_state.uc_stack.ss_sp = new_pcb->stack;
  new_pcb->cpu_state.uc_stack.ss_size = STACK_SIZE;
  new_pcb->cpu_state.uc_stack.ss_flags = 0;
  sigemptyset(&new_pcb->cpu_state.uc_sigmask);
  new_pcb->cpu_state.uc_link = &main_context;
  void (*new_function) (void*)= (void(*)(void*))  running->syscall_args[0];
  makecontext(&new_pcb->cpu_state, (void(*)())  new_function, 1, (void*)running->syscall_args[1]);
}