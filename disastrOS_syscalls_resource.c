#include <assert.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_fifo.h"
#include "disastrOS_descriptor.h"

void internal_open(){
  // 1. Retrieve argument from PCB  
  int resource_id = running->syscall_args[0];
  int flags = running->syscall_args[1];

  // 2. Call open function
  int ret = Resource_open(resource_id, flags);
  if(ret == DSOS_ERESTARTNOINTR) return;
  // 3. Write result to the PCB and return.
  running->syscall_retvalue = ret;
  return;
}

void internal_read(){
  // 1. Retrieve argument from PCB  
  int fd = running->syscall_args[0];
  void* buffer = (void*) running->syscall_args[1];
  int count = running->syscall_args[2];
  // 2. Call read function
  int ret = Resource_read(fd, buffer, count);
  // 3. Write result to the PCB and return.
  if(ret == DSOS_ERESTARTNOINTR) return;
  running->syscall_retvalue = ret;
  return;
}

void internal_write(){
  // 1. Retrieve argument from PCB  
  int fd = running->syscall_args[0];
  void* buffer = (void*) running->syscall_args[1];
  int count = running->syscall_args[2];

  // 2. Call read function
  int ret = Resource_write(fd, buffer, count);

  // 3. Write result to the PCB and return.
  if(ret == DSOS_ERESTARTNOINTR) return;
  
  running->syscall_retvalue = ret;
  
  return;
}

void internal_close(){
  // 1. Retrieve argument from PCB  
  int fd = running->syscall_args[0];

  // 2. Call close function
  int ret = Resource_close(fd);

  // 3. Write result to the PCB and return.
  running->syscall_retvalue = ret;
  return;
}

void internal_unlink(){
  // 1. Retrieve argument from PCB  
  int resource_id = running->syscall_args[0];

  // 2. Call unlink function
  int ret = Resource_unlink(resource_id);

  // 3. Write result to the PCB and return.
  running->syscall_retvalue = ret;
  return;
}

void internal_mkresource(){
  // 1. Retrieve argument from PCB  
  int resource_id = running->syscall_args[0];

  // 2. Call mk function
  int ret = Resource_mk(resource_id);

  // 3. Write result to the PCB and return.
  running->syscall_retvalue = ret;
  return;
}

void internal_mkfifo(){
  // 1. Retrieve argument from PCB  
  int resource_id = running->syscall_args[0];

  // 2. Call mk function
  int ret = Fifo_mk(resource_id);

  // 3. Write result to the PCB and return.
  running->syscall_retvalue = ret;
  return;
}

void internal_mkpipe(){
  // 1. Retrieve argument from PCB  
  void* pipefd = (void*) running->syscall_args[0];

  // 2. Call Pipe_mk function
  int ret = Pipe_mk(pipefd);

  // 3. Write result to the PCB and return.
  running->syscall_retvalue = ret;
  return;
}
