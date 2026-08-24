#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_descriptor.h"

// COMMENTARE QUESTO CODICE CON I NUMERI

// The original implementation had a problem:
// resources and timers were released if and only if the parent was in the *waiting* state
// (i.e., it had already called `disastrOS_wait`).
// 

// called upon termination
// moves the process to a zombie status
// if the process has children they need to be reparented to
// the init process (pid 1);
// all children should receive SIGHUP
// parent should receive SIGCHLD and be awakened if it was waiting
// for this process to report its value
void internal_exit(){
  printf("Entro nella exit\n");
  // 2nd register in pcb contains the exit value
  running->return_value=running->syscall_args[0];
    
  assert(init_pcb);
  while(running->children.first){

    // detach from current list
    ListItem* item = List_detach(& running->children, running->children.first);
    assert(item);
    
    // attach to init's children list
    List_insert(& init_pcb->children, init_pcb->children.last, item);

    // send SIGHUP
    PCBPtr* pcb_ptr=(PCBPtr*) item;
    PCB* pcb=pcb_ptr->pcb;
    pcb->signals |= (DSOS_SIGHUP & pcb->signals_mask);
  }

  // we release all timers put by this process from the timer list, in case there was someone
  // hanging
  // WARNING: untested
  ListItem* aux=timer_list.first;
  while(aux){
    TimerItem* timer=(TimerItem*) aux;
    aux=aux->next;
    if (timer->pcb==running){
	    ListItem* detach_result=List_detach(&timer_list, (ListItem*)timer);
  	  assert(detach_result);
	    TimerItem_free(timer);
    }
  }

  // we release all resources of a process upon termination
  int i=0;
  printf("Prima di liberare i descrittori ci sono spazi liberi nell'allocatore: %d descrittori", Descriptor_allocator_getinfo()->buffer_size);
  printf("Size lista descrittori letta dentro la exit (prima del while) %d\n", running->descriptors.size);
  while(running->descriptors.first) {
    Descriptor* descriptor = (Descriptor*) running->descriptors.first;
    printf("CHIUDO NELLA EXIT! Iterazione %d, descrittore: %p\n", i++, descriptor);
    int close_ret = Resource_close(descriptor->fd);
    printf("Valore ritorno close: %d\n", close_ret);
    assert(!close_ret && "Fatal error during exit (close resource). Kernel Panic!");
  }
  printf("Dopo liberati i descrittori ci sono spazi liberi nell'allocatore: %d descrittori", Descriptor_allocator_getinfo()->buffer_size);

  running->status=Zombie;
  List_insert(&zombie_list, zombie_list.last, (ListItem*) running);
  running->parent->signals |= (DSOS_SIGCHLD & running->parent->signals_mask);

  // if the parent was waiting for this process to die
  if (running->parent->status==Waiting
      // since he called  wait or waitpid, we wake him up
      && running->parent->syscall_num==DSOS_CALL_WAIT
      && (running->parent->syscall_args[0]==0 ||
	    running->parent->syscall_args[0]==running->pid)
  ){
    PCB* parent= (PCB*) List_detach(&waiting_list, (ListItem*) running->parent);
    assert(parent);
    parent->status=Running;

    // we remove the process from the parent's children list
    PCBPtr* self_in_parent=PCBPtr_byPID(&parent->children, running->pid);
    List_detach(&parent->children, (ListItem*) self_in_parent);
    
    // hack the PCB of the parent to put in return value
    parent->syscall_retvalue=running->pid;
    int* result=(int*)parent->syscall_args[1];
    if (result)
      *result=running->return_value;
    
    // the process finally dies
    ListItem* suppressed_item = List_detach(&zombie_list, (ListItem*) running);
    PCB_free((PCB*) suppressed_item);
    running=parent;
  } else {
    // we put the first ready process in running 
    PCB* next_running=(PCB*) List_detach(&ready_list, ready_list.first);
    assert(next_running);
    next_running->status=Running;
    running=next_running;
  }
}
