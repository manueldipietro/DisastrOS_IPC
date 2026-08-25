#include "tester_fifo.h"
#include "tester.h"

#include "disastrOS_constants.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tester_utest_fifo(){
    int is_all_test_ok = 1;

    tester_utest_list utest_mk[] = {
        { .title = "Test disastrOS_fifo: mk1:", .utest_fn = tester_utest_fifo_mk1},
        { .title = "Test disastrOS_fifo: mk2:", .utest_fn = tester_utest_fifo_mk2},
        { .title = "Test disastrOS_fifo: mk3:", .utest_fn = tester_utest_fifo_mk3},
        { .title = "Test disastrOS_fifo: mk4:", .utest_fn = tester_utest_fifo_mk4}
    };

    tester_utest_list utest_onopen[] = {
        { .title = "Test disastrOS_fifo: onopen1: invalid flag DSOS_O_RDWR", .utest_fn = tester_utest_fifo_onopen1},
        { .title = "Test disastrOS_fifo: onopen2: blocking reader opener", .utest_fn = tester_utest_fifo_onopen2},
        { .title = "Test disastrOS_fifo: onopen3: unblock readers opener after opener writer", .utest_fn = tester_utest_fifo_onopen3},
        { .title = "Test disastrOS_fifo: onopen4: blocking writer opener", .utest_fn = tester_utest_fifo_onopen4},
        { .title = "Test disastrOS_fifo: onopen5: unblocking writers opener after opener", .utest_fn = tester_utest_fifo_onopen5},
        { .title = "Test disastrOS_fifo: onopen6: DSOS_O_NONBLOCKING readers opener without writers", .utest_fn = tester_utest_fifo_onopen6},
        { .title = "Test disastrOS_fifo: onopen7: DSOS_O_NONBLOCKING writers opener without readers", .utest_fn = tester_utest_fifo_onopen7},
        { .title = "Test disastrOS_fifo: onopen8: DSOS_O_NONBLOCKING readers opener with writers", .utest_fn = tester_utest_fifo_onopen8},
        { .title = "Test disastrOS_fifo: onopen9: DSOS_O_NONBLOCKING writers opener with readers", .utest_fn = tester_utest_fifo_onopen9},
    };

    tester_utest_list utest_readandwrite[] = {
        { .title = "Test disastrOS_fifo: mk1:", .utest_fn = tester_utest_fifo_read1},
    };


    printf("Executing utest for resource module:\n");
    is_all_test_ok *= tester_utest_executelist(utest_mk, sizeof(utest_mk)/sizeof(utest_mk[0]), "fifo_mk") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_onopen, sizeof(utest_onopen)/sizeof(utest_onopen[0]), "fifo_onopen") ? 1 : 0;
    is_all_test_ok *= tester_utest_executelist(utest_readandwrite, sizeof(utest_readandwrite)/sizeof(utest_readandwrite[0]), "fifo_read and fifo_write") ? 1 : 0;

    return is_all_test_ok;
}

void tester_utest_fifo_utils_sleeper(){
    while(1) {
        getc(stdin);
        disastrOS_printStatus();
    }
    return;
}

int tester_utest_fifo_utils_reader_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval){
    // 0. Initialize
    int return_value, file_descriptor, flags;

    // 1. Set flags and open resource
    flags = DSOS_O_RDONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    file_descriptor = return_value;

    // 2. 
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the son from open (with code %d)", return_value);
        exit(0);
    }

    // 3.
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_open_retval, file_descriptor, "(on writer_open son) error on disastrOS_open"));

    // 4. All ok, return 1
    return 1;
}

int tester_utest_fifo_utils_writer_open(int resource_id, int should_exit, int non_blocking, int expected_open_retval){
    // 0. Initialize
    int return_value, file_descriptor, flags;

    // 1. Set flags and open resource
    flags = DSOS_O_WRONLY;
    if(non_blocking){flags = flags | DSOS_O_NONBLOCK;}
    return_value = disastrOS_open(resource_id, flags);
    file_descriptor = return_value;

    // 2. 
    if(should_exit == 0){
        snprintf(TESTER_UTEST_FAILMSG, TESTER_UTEST_FAILMSG_SIZE, "unexpected return of the son from open (with code %d)", return_value);
        exit(0);
    }

    // 3.
    TESTER_UTEST_CHECK(tester_utest_assert_ecodege(expected_open_retval, file_descriptor, "(on writer_open son) error on disastrOS_open"));

    // 4. All ok, return 1
    return 1;
}


void test_itest_fifo_packet_reader(void* arguments){
    // 0. Retrieve arguments
    int reader_id = ((Arguments_fifo_packet_reader*) arguments)->reader_id;
    int resource_id = ((Arguments_fifo_packet_reader*) arguments)->resource_id;
    
    // 1. Open FIFO in DSOS_O_RDONLY mode
    printf("READER %d| Startup\n", reader_id);
    int fd = disastrOS_open(resource_id, DSOS_O_RDONLY);
    if(fd<0){
            printf("READER %d| disastrOS_open returned %d\n", reader_id, fd);
            disastrOS_exit(-1);
    }
    printf("LETTORE APERTO!\n");
    // 2. Read packet 
    int ret_val, received = 0;
    char buffer[7];
    do{
        printf("Entro nella disastrOS_read nel lettore\n");
        ret_val = disastrOS_read(fd, buffer, 6);
        printf("\nRitorno dalla read nel lettore\n");
        if(ret_val != 6 && ret_val != 0){
            printf("READER %d| disastrOS_read returned %d, expected 6\n", reader_id, ret_val);
            disastrOS_exit(-1);
        }
        if(ret_val == 6 && memcmp("PACKET", buffer, 6)){
            printf("READER %d| Error in received packet: %.6s\n", reader_id, buffer);
            disastrOS_exit(-1);
        }
        ++received;
        printf("READER %d| Sended one packet, total packet received: %d\n", reader_id, received);
    }while(ret_val != 0);

    // 3. Close fd and return
    printf("READER %d| Ended, total packet received: %d\n", reader_id, received);
    disastrOS_close(fd);
    disastrOS_exit(received);
    return;
}

void test_itest_fifo_packet_writer(void* arguments){
    // 0. Retrieve arguments
    int writer_id = ((Arguments_fifo_packet_writer*) arguments)->writer_id;
    int resource_id = ((Arguments_fifo_packet_writer*) arguments)->resource_id;
    int to_send = ((Arguments_fifo_packet_writer*) arguments)->to_send;
    
    // 1. Open FIFO in DSOS_O_RDONLY mode
    printf("WRITER %d| Startup\n", writer_id);
    int fd = disastrOS_open(resource_id, DSOS_O_WRONLY);
    if(fd<0){
        printf("WRITER %d| disastrOS_open returned %d\n", writer_id, fd);
        disastrOS_exit(-1);
    }

    // 2. Write to_send packet to the buffer
    const char buffer[7] = "PACKET";
    int sended = 0;
    while(sended<to_send){
        int ret_val = disastrOS_write(fd, buffer, 6);
        if(ret_val != 6){
            printf("WRITER %d| disastrOS_write returned %d, expected 6\n", writer_id, ret_val);
            disastrOS_exit(-1);
        }
        sended++;
        printf("WRITER %d| Received one packet, total packet sended: %d\n", writer_id, sended);
    }

    // 3. Close fd and return
    printf("WRITER %d| Ended, total packet sended: %d\n", writer_id, sended);
    disastrOS_close(fd);
    disastrOS_exit(sended);
}

void test_itest_fifo2_init(){
    printf("INTEGRATION TEST 1 Producer - N Consumers\n");
    printf("Producer will produce 100 packet and consumers will consume it\n");
    printf("At the end the number of produced packet will be the same of the total consumption packet");

    // 1. Create the fifo
    int ret_val = disastrOS_mkfifo(10);
    if(ret_val != DSOS_SUCCESS){
        printf("Error, disastrOS_make return %d, expected 0\n", ret_val);
        return;
    }

    // 2. Spawn 1 writer (producer)
    Arguments_fifo_packet_writer writer_arguments = {
        .writer_id = 1,
        .resource_id = 10,
        .to_send = 100,
    };
    int writer_pid = last_pid;
    disastrOS_spawn(test_itest_fifo_packet_writer, &writer_arguments);

    // 3. Spawn 10 reader (consumer)
    Arguments_fifo_packet_reader reader_arguments[10];
    for(int i=0; i<10; i++){
        reader_arguments[i].reader_id = i+1;
        reader_arguments[i].resource_id = 10;
        disastrOS_spawn(test_itest_fifo_packet_reader, &reader_arguments[i]);
        printf("CICLO %d\n", i);
    }

    // 4.Wait for writer
    int pid;
    pid = disastrOS_wait(writer_pid, &ret_val);
    if(ret_val > -1)
        printf("The writer has send: %d packet.\n", ret_val);
    else return;

    // 5. Wait for readers
    int readed = 0;
    for(int i=0; i<10; i++){
        pid = disastrOS_wait(0, &ret_val);
        if(ret_val == -1){
            printf("Error in one son, return;\n");
            return;
        }
        readed += ret_val;
    }
    printf("The reader has received: %d packet.\n", ret_val);

    // 6. Return
    return;
}


// 1 Producer - 1 Consumer
void test_itest_fifo1_init(){
    printf("INTEGRATION TEST 1 Producer - 1 Consumer\n");
    printf("Transfer a text from Producer to consumer.");
}




void test_itest_fifo3_init(){
    printf("INTEGRATION TEST N Producer - 1 Consumer\n");
    printf("Producer will produce packet of 10 byte and consumer will consume it\n");
    printf("At the end the number of produced packet will be the same of the total consumption packet");
    return;
}

void test_itest_fifo4_init(){
    printf("INTEGRATION TEST N Producer - N Consumer\n");
    printf("Producer will produce packet of 10 byte and consumer will consume it\n");
    printf("At the end the number of produced packet will be the same of the total consumption packet");
    return;
}

