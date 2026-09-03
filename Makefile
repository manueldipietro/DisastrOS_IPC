CC=gcc
CCOPTS=--std=gnu99 -Wall -g -I . -I tester
AR=ar

HEADERS=disastrOS.h\
	disastrOS_constants.h\
	disastrOS_descriptor.h\
	disastrOS_globals.h\
	disastrOS_pcb.h\
	disastrOS_resource.h\
     disastrOS_ipc.h\
     disastrOS_fifo.h\
	disastrOS_syscalls.h\
	disastrOS_timer.h\
	linked_list.h\
	pool_allocator.h\
     \
     tester/tester_ipc.h\
     tester/tester_fifo.h\
     \
     

OBJS=pool_allocator.o\
     linked_list.o\
     circular_buffer.o\
     disastrOS_timer.o\
     disastrOS_pcb.o\
     disastrOS_resource.o\
     disastrOS_descriptor.o\
     disastrOS.o\
     disastrOS_fork.o\
     disastrOS_wait.o\
     disastrOS_spawn.o\
     disastrOS_exit.o\
     disastrOS_shutdown.o\
     disastrOS_schedule.o\
     disastrOS_preempt.o\
     disastrOS_sleep.o\
     disastrOS_syscalls_resource.o\
     disastrOS_ipc.o\
     disastrOS_fifo.o\
     \
     \
     tester/tester.o\
     tester/tester_aux.o\
     tester/tester_ipc.o\
     tester/tester_fifo.o\
     \
     tester/tester_utest_resource/tester_resource_mk.o\
     tester/tester_utest_resource/tester_resource_open.o\
     tester/tester_utest_resource/tester_resource_unlink.o\
     tester/tester_utest_resource/tester_resource_close.o\
     tester/tester_utest_resource/tester_resource_read.o\
     tester/tester_utest_resource/tester_resource_write.o\
     \
     tester/tester_utest_ipc/tester_utest_ipc_mk.o\
     tester/tester_utest_ipc/tester_utest_ipc_read.o\
     tester/tester_utest_ipc/tester_utest_ipc_write.o\
     \
     tester/tester_circular_buffer.o\
     \
     tester/tester_utest_fifo/tester_utest_fifo_mk.o\
     tester/tester_utest_fifo/tester_utest_fifo_onopen.o\
     tester/tester_utest_fifo/tester_utest_fifo_readandwrite.o\
     tester/tester_utest_fifo/tester_utest_fifo_onclose.o\
     \
     tester/tester_spawnfd.o\
     \
     tester/tester_itest/tester_itest_pipe.o\
     tester/tester_itest/tester_itest_fifo.o\
     tester/tester_itest/tester_itest_resource.o\
     tester/tester_utest_executor.o\

LIBS=libdisastrOS.a

BINS=disastrOS_test

#disastros_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libdisastrOS.a: $(OBJS) $(HEADERS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

disastrOS_test:		disastrOS_test.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
