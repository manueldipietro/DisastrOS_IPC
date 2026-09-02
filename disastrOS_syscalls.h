#pragma once
#include <assert.h>
#include "disastrOS.h"
#include "disastrOS_globals.h"

void internal_preempt();
void internal_fork();
void internal_exit();
void internal_wait();
void internal_spawn();
void internal_spawn_withfd();
void internal_shutdown();
void internal_schedule();
void internal_sleep();

// Syscalls resources
void internal_open();
void internal_read();
void internal_write();
void internal_close();
void internal_unlink();
void internal_mkresource();
void internal_mkfifo();
void internal_mkpipe();
