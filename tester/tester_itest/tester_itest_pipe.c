#include "tester_fifo.h"
#include "tester_aux.h"

#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* tester_itest_pipe_example_text =
    "Quel ramo del lago di Como, che volge a mezzogiorno, tra due "
    "catene non interrotte di monti, tutto a seni e a golfi, a seconda "
    "dello sporgere e del rientrare di quelli, vien, quasi a un tratto, "
    "a ristringersi, e a prender corso e figura di fiume, tra un "
    "promontorio a destra, e un'ampia costiera dall'altra parte; e il "
    "ponte, che ivi congiunge le due rive, par che renda ancor più "
    "sensibile all'occhio questa trasformazione, e segni il punto in "
    "cui il lago cessa, e l'Adda rincomincia, per ripigliar poi nome "
    "di lago dove le rive, allontanandosi di nuovo, lascian l'acqua "
    "distendersi e rallentarsi in nuovi golfi e in nuovi seni. La "
    "costiera, formata dal deposito di tre grossi torrenti, scende "
    "appoggiata a due monti contigui, l'uno detto di san Martino, l'altro, "
    "con voce lombarda, il Resegone, dai molti suoi cocuzzoli in fila, "
    "che in vero lo fanno somigliare a una sega: talchè non è chi, al "
    "primo vederlo, purchè sia di fronte, come per esempio di su le mura "
    "di Milano che guardano a settentrione, non lo discerna tosto, a un "
    "tal contrassegno, in quella lunga e vasta giogaia, dagli altri "
    "monti di nome più oscuro e di forma più comune.";

typedef struct Arguments_pipe_text_writer{int text_len; const char* text_buffer; int pipefd[2];} Arguments_pipe_text_writer;
void test_itest_pipe_text_writer(void* arguments){
    // 0. Retrieve base arguments
    int text_len = ((Arguments_pipe_text_writer*) arguments)->text_len;
    const char* text_buffer = ((Arguments_pipe_text_writer*) arguments)->text_buffer;
    // 1. Retrieve file descriptors int from arguments and close the read descriptor
    int return_value = disastrOS_close(((Arguments_pipe_text_writer*) arguments)->pipefd[DSOS_PIPE_RD]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_WRITER_TAG "disastrOS_close (during close RD descriptor) return %d\n", return_value);
        disastrOS_exit(-1);
    }
    int fd = ((Arguments_pipe_text_writer*) arguments)->pipefd[DSOS_PIPE_WR];
    // 2. Write on the buffer
    int writted = disastrOS_write(fd, text_buffer, text_len);
    printf(TESTER_WRITER_TAG "ended\n");
    // 3. Close write descriptor and return number of written byte
    return_value = disastrOS_close(fd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_WRITER_TAG "disastrOS_close (during close WR descriptor at the end) return %d\n", return_value);
        disastrOS_exit(-1);
    }
    disastrOS_exit(writted);
}

typedef struct Arguments_pipe_text_reader{int text_len; char* text_buffer; int pipefd[2];} Arguments_pipe_text_reader;
void test_itest_pipe_text_reader(void* arguments){
    // 0. Retrieve base arguments
    int text_len = ((Arguments_pipe_text_reader*) arguments)->text_len;
    char* text_buffer = ((Arguments_pipe_text_reader*) arguments)->text_buffer;
    // 1. Retrieve file descriptors int from arguments and close the write descriptor
    int return_value = disastrOS_close(((Arguments_pipe_text_reader*) arguments)->pipefd[DSOS_PIPE_WR]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_READER_TAG "disastrOS_close (during close WR descriptor) return %d\n", return_value);
        disastrOS_exit(-1);
    }
    int fd = ((Arguments_pipe_text_reader*) arguments)->pipefd[DSOS_PIPE_RD];
    // 2. Read text
    int ret_val, readed = 0;
    while(1){
        ret_val = disastrOS_read(fd, text_buffer+readed, text_len-readed);
        if(ret_val == 0) break;
        if(ret_val < 0){
            printf(TESTER_READER_TAG "error on disastrOS_read\n");
            disastrOS_exit(-1);
        }
        readed += ret_val;
    }
    text_buffer[readed] = '\0';
    // 3. Close read descriptor and return number of readed byte
    printf(TESTER_READER_TAG "ended, EOF detected\n");
    return_value = disastrOS_close(fd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_READER_TAG "disastrOS_close (during close RD descriptor at the end) return %d\n", return_value);
        disastrOS_exit(-1);
    }
    disastrOS_exit(readed);
}

typedef struct Arguments_pipe_packet_writer{int writer_id; int to_send; int sleep_time; int sleep_rate; int pipefd[2];}Arguments_pipe_packet_writer;
void test_itest_pipe_packet_writer(void* arguments){
    // 0. Retrieve arguments
    int writer_id = ((Arguments_pipe_packet_writer*) (arguments))->writer_id;
    int to_send = ((Arguments_pipe_packet_writer*) (arguments))->to_send;
    int sleep_time = ((Arguments_pipe_packet_writer*) (arguments))->sleep_time;
    int sleep_rate = ((Arguments_pipe_packet_writer*) (arguments))->sleep_rate;
    // 1. Retrieve file descriptors int from arguments and close the read descriptor
    int return_value = disastrOS_close(((Arguments_pipe_packet_writer*) arguments)->pipefd[DSOS_PIPE_RD]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_WRITER_TAG_WID "disastrOS_close (during close RD descriptor) return %d\n", writer_id, return_value);
        disastrOS_exit(-1);
    }
    int fd = ((Arguments_pipe_packet_writer*) arguments)->pipefd[DSOS_PIPE_WR];
    // 2. Write packet on the pipe
    const char buffer[7] = "PACKET";
    int sended = 0;
    while(sended<to_send){
        return_value = disastrOS_write(fd, buffer, 6);
        if(return_value != 6){
            printf(TESTER_WRITER_TAG_WID "disastrOS_write returned %d, expected 6\n", writer_id, return_value);
            disastrOS_exit(-1);
        }
        sended++;
        printf(TESTER_WRITER_TAG_WID "received one packet, total packet sended: %d\n", writer_id, sended);
        if(sended % sleep_rate == 0) disastrOS_sleep(sleep_time);
    }
    // 3. Close write descriptor and return number of written byte
    return_value = disastrOS_close(fd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_WRITER_TAG "disastrOS_close (during close WR descriptor at the end) return %d\n", return_value);
        disastrOS_exit(-1);
    }
    disastrOS_exit(sended);
}

typedef struct Arguments_pipe_packet_reader{int reader_id; int sleep_time; int sleep_rate; int pipefd[2];}Arguments_pipe_packet_reader;
void test_itest_pipe_packet_reader(void* arguments){
    // 0. Retrieve arguments
    int reader_id = ((Arguments_pipe_packet_reader*) (arguments))->reader_id;
    int sleep_time = ((Arguments_pipe_packet_reader*) (arguments))->sleep_time;
    int sleep_rate = ((Arguments_pipe_packet_reader*) (arguments))->sleep_rate;
    // 1. Retrieve file descriptors int from arguments and close the write descriptor
    int return_value = disastrOS_close(((Arguments_pipe_packet_reader*) arguments)->pipefd[DSOS_PIPE_WR]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_READER_TAG_WID "disastrOS_close (during close RD descriptor) return %d\n", reader_id, return_value);
        disastrOS_exit(-1);
    }
    int fd = ((Arguments_pipe_packet_reader*) arguments)->pipefd[DSOS_PIPE_RD];
    // 2. Read packet from the pipe
    int received = 0;
    char buffer[7];
    while(1){
        return_value = disastrOS_read(fd, buffer, 6);
        if(return_value == 0)break;
        if(return_value != 6){
            printf(TESTER_READER_TAG_WID "disastrOS_read returned %d, expected 6\n", reader_id, return_value);
            disastrOS_exit(-1);
        }
        if(memcmp("PACKET", buffer, 6) != 0){
            printf(TESTER_READER_TAG_WID "error in received packet: %.6s\n", reader_id, buffer);
            disastrOS_exit(-1);
        }
        ++received;
        printf(TESTER_READER_TAG_WID "received one packet, total packet received: %d\n", reader_id, received);
        if(received % sleep_rate == 0) disastrOS_sleep(sleep_time);
    }
    // 3. Close write descriptor and return number of written byte
    return_value = disastrOS_close(fd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_READER_TAG_WID "disastrOS_close (during close RD descriptor at the end) return %d\n", reader_id, return_value);
        disastrOS_exit(-1);
    }
    disastrOS_exit(received);
}

void test_itest_pipe1_init(){
    // 0. Print test header
    printf("----------------------------------------------------------------------------------------------\n");
    printf(TESTER_ANSI_BRIGHT_RED TESTER_ANSI_BOLD "INTEGRATION TEST 1 Producer - 1 Consumer\n" TESTER_ANSI_RESET);
    printf("Transfer a text from Producer to consumer (single write with len > 2*PIPE_BUF)\n");
    printf("Text will print the incipit of \"Prosmessi Sposi\" Book\n");
    printf("----------------------------------------------------------------------------------------------\n");
    // 1. Create the pipe and spawn the sleeper
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);    // DA CAMBIARE CON SLEEP AUSILIARIO GENERALE
    int pipefd[2];
    int return_value = disastrOS_mkpipe(pipefd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "error, disastrOS_make return %d, expected 0\n", return_value);
        return;
    }
    // 2. Spawn 1 writer (producer)
    Arguments_pipe_text_writer writer_arguments = {
        .text_buffer = tester_itest_pipe_example_text,
        .text_len= strlen(tester_itest_pipe_example_text),
        .pipefd[0] = pipefd[0],
        .pipefd[1] = pipefd[1]
    };
    int writer_pid = last_pid;
    disastrOS_spawn_withfd(test_itest_pipe_text_writer, &writer_arguments);
    printf(TESTER_INIT_TAG "spawned writer\n");
    // 3. Spawn 1 reader (consumer)
    char readed_text[strlen(tester_itest_pipe_example_text)];
    Arguments_pipe_text_reader reader_arguments = {
        .text_buffer = readed_text,
        .text_len = strlen(tester_itest_pipe_example_text),
        .pipefd[0] = pipefd[0],
        .pipefd[1] = pipefd[1]
    };
    int reader_pid = last_pid;
    disastrOS_spawn_withfd(test_itest_pipe_text_reader, &reader_arguments);
    printf(TESTER_INIT_TAG "spawned reader\n");
    // 4. Close descriptors
    return_value = disastrOS_close(pipefd[DSOS_PIPE_RD]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close RD descriptor at the end) return %d\n", return_value);
    }
    return_value = disastrOS_close(pipefd[DSOS_PIPE_WR]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close WR descriptor at the end) return %d\n", return_value);
    }
    // 5.Wait for writer
    int pid, writed;
    pid = disastrOS_wait(writer_pid, &writed);
    if(pid < 0){
        printf(TESTER_INIT_TAG "error in writer wait\n");
        return;
    }
    if(writed > -1){
        printf(TESTER_INIT_TAG "exited writer\n");
    }
    else{
        printf(TESTER_INIT_TAG "error in the writer (detected with wait), exit from the test\n");
        return;
    }
    // 6.Wait for reader
    pid = disastrOS_wait(reader_pid, &return_value);
    if(pid < 0){
        printf(TESTER_INIT_TAG "error in reader wait\n");
        return;
    }
    if(return_value > -1){
        printf(TESTER_INIT_TAG "exited reader\n");
        printf(TESTER_INIT_TAG "the writer has send: %d packet\n", writed);
        printf(TESTER_INIT_TAG "the reader has received: %d packet\n", return_value);
        printf(TESTER_INIT_TAG "the received text is:\n");
        printf("%s\n", readed_text);
    }
    else{
        printf(TESTER_INIT_TAG "error in the reader (detected with wait), exit from the test\n");
        return;
    }
    // 7. Return
    printf("----------------------------------------------------------------------------------------------\n");
    return;
}

void test_itest_pipe2_init(){
    printf("----------------------------------------------------------------------------------------------\n");
    printf(TESTER_ANSI_BRIGHT_RED TESTER_ANSI_BOLD "INTEGRATION TEST 1 Producer - N Consumers\n" TESTER_ANSI_RESET);
    printf("Producer will produce 100 packet each and consumers will consume it\n");
    printf("At the end the number of produced packet will be the same of the total consumption packet");
    printf("Spawn 1 writers (produce 10 packet each) and 10 readers (will receive 100 packet total)\n");
    printf("----------------------------------------------------------------------------------------------\n");
    // 1. Create the pipe and spawn the sleeper
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    int pipefd[2];
    int return_value = disastrOS_mkpipe(pipefd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "error, disastrOS_make return %d, expected 0\n", return_value);
        return;
    }
    // 2. Spawn 1 writer (producer)
    Arguments_pipe_packet_writer writer_arguments = {
        .writer_id = 1,
        .to_send = 100,
        .sleep_rate = 5,
        .sleep_time = 3,
        .pipefd[0] = pipefd[0],
        .pipefd[1] = pipefd[1]
    };
    int writer_pid = last_pid;
    disastrOS_spawn_withfd(test_itest_pipe_packet_writer, &writer_arguments);
    printf(TESTER_INIT_TAG "spawned writer\n");
    // 3. Spawn 10 readers (consumers)
    int readers_pid[10];
    Arguments_pipe_packet_reader readers_arguments[10];
    for(int i=0; i<10; i++){
        readers_arguments[i].reader_id = i+1;
        readers_arguments[i].sleep_time = 4;
        readers_arguments[i].sleep_rate = 5;
        readers_arguments[i].pipefd[DSOS_PIPE_RD] = pipefd[DSOS_PIPE_RD];
        readers_arguments[i].pipefd[DSOS_PIPE_WR] = pipefd[DSOS_PIPE_WR];
        readers_pid[i] = last_pid;
        disastrOS_spawn_withfd(test_itest_pipe_packet_reader, &readers_arguments[i]);
        printf(TESTER_INIT_TAG "spawned reader: %d\n", i+1);
    }
    // 4. Close descriptors
    return_value = disastrOS_close(pipefd[DSOS_PIPE_RD]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close RD descriptor at the end) return %d\n", return_value);
    }
    return_value = disastrOS_close(pipefd[DSOS_PIPE_WR]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close WR descriptor at the end) return %d\n", return_value);
    }
    // 5.Wait for writer
    int pid;
    pid = disastrOS_wait(writer_pid, &return_value);
    if(pid < 0){
        printf(TESTER_INIT_TAG "error in writer wait\n");
        return;
    }
    if(return_value > -1){
        printf(TESTER_INIT_TAG "exited writer\n");
    }
    else{
        printf(TESTER_INIT_TAG "error in the writer (detected with wait), exit from the test;\n");
        return;
    }
    // 6. Wait for readers
    int readed = 0;
    for(int i=0; i<10; i++){
        pid = disastrOS_wait(readers_pid[i], &return_value);
        if(pid < 0){
            printf(TESTER_INIT_TAG "error in readers wait\n");
            return;
        }
        if(return_value == -1){
            printf(TESTER_INIT_TAG "error in one reader (detected with wait), exit from the test;\n");
            return;
        }
        readed += return_value;
        printf(TESTER_INIT_TAG "exited reader: %d\n", i+1);
    }
    // 7. Return
    printf(TESTER_INIT_TAG "the readers received in total: %d packet.\n", readed);
    printf("----------------------------------------------------------------------------------------------\n");
    return;
}

void test_itest_pipe3_init(){
    printf("----------------------------------------------------------------------------------------------\n");
    printf(TESTER_ANSI_BRIGHT_RED TESTER_ANSI_BOLD "INTEGRATION TEST N Producer - 1 Consumers\n" TESTER_ANSI_RESET);
    printf("Producer will produce 100 packet and consumers will consume it\n");
    printf("At the end the number of produced packet will be the same of the total consumption packet");
    printf("Spawn N writers (produce 10 packet each) and 1 readers (will receive 100 packet total)\n");
    printf("----------------------------------------------------------------------------------------------\n");
    // 1. Create the pipe and spawn the sleeper
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    int pipefd[2];
    int return_value = disastrOS_mkpipe(pipefd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "error, disastrOS_make return %d, expected 0\n", return_value);
        return;
    }
    // 2. Spawn 10 writers (producers)
    int writers_pid[10];
    Arguments_pipe_packet_writer writers_arguments[10];
    for(int i=0; i<10; i++){
        writers_arguments[i].writer_id = i+1;
        writers_arguments[i].to_send = 10;
        writers_arguments[i].sleep_time = 4;
        writers_arguments[i].sleep_rate = 5;
        writers_arguments[i].pipefd[DSOS_PIPE_RD] = pipefd[DSOS_PIPE_RD];
        writers_arguments[i].pipefd[DSOS_PIPE_WR] = pipefd[DSOS_PIPE_WR];
        writers_pid[i] = last_pid;
        disastrOS_spawn_withfd(test_itest_pipe_packet_writer, &writers_arguments[i]);
        printf(TESTER_INIT_TAG "spawned writer: %d\n", i+1);
    }
    // 3. Spawn 1 reader (consumer)
    Arguments_pipe_packet_reader reader_arguments = {
        .reader_id = 1,
        .sleep_time = 4,
        .sleep_rate = 5,
        .pipefd[0] = pipefd[0],
        .pipefd[1] = pipefd[1]
    };
    int reader_pid = last_pid;
    disastrOS_spawn_withfd(test_itest_pipe_packet_reader, &reader_arguments);
    printf(TESTER_INIT_TAG "spawned reader\n");
    // 4. Close descriptors
    return_value = disastrOS_close(pipefd[DSOS_PIPE_RD]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close RD descriptor at the end) return %d\n", return_value);
    }
    return_value = disastrOS_close(pipefd[DSOS_PIPE_WR]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close WR descriptor at the end) return %d\n", return_value);
    }
    // 5. Wait for writers
    int pid;
    int writed = 0;
    for(int i=0; i<10; i++){
        pid = disastrOS_wait(writers_pid[i], &return_value);
        if(pid < 0){
            printf(TESTER_INIT_TAG "error in writers wait\n");
            return;
        }
        if(return_value == -1){
            printf(TESTER_INIT_TAG "error in one writer (detected with wait), exit from the test;\n");
            return;
        }
        writed += return_value;
        printf(TESTER_INIT_TAG "exited writer: %d\n", i+1);
    }
    // 6.Wait for reader
    pid = disastrOS_wait(reader_pid, &return_value);
    if(pid < 0){
        printf(TESTER_INIT_TAG "error in readers wait\n");
        return;
    }
    if(return_value > -1){
        printf(TESTER_INIT_TAG "sxited reader\n");
        printf(TESTER_INIT_TAG "the writers have send: %d packet.\n", writed);
        printf(TESTER_INIT_TAG "the reader has received: %d packet.\n", return_value);
    }
    else{
        printf(TESTER_INIT_TAG "error in the reader (detected with wait), exit from the test;\n");
        return;
    }
    // 7. Return
    printf("----------------------------------------------------------------------------------------------\n");
    return;
}

void test_itest_pipe4_init(){
    printf("----------------------------------------------------------------------------------------------\n");
    printf(TESTER_ANSI_BRIGHT_RED TESTER_ANSI_BOLD "INTEGRATION TEST N Producer - N Consumers\n" TESTER_ANSI_RESET);
    printf("Producers will produce 10 packet each and consumers will consume it\n");
    printf("At the end the number of produced packet will be the same of the total consumption packet");
    printf("Spawn 10 writers (produce 10 packet each) and 10 readers (will receive 100 packet total)\n");
    printf("----------------------------------------------------------------------------------------------\n");
    // 1. Create the pipe and spawn the sleeper
    disastrOS_spawn(tester_utest_fifo_utils_sleeper, 0);
    int pipefd[2];
    int return_value = disastrOS_mkpipe(pipefd);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "error, disastrOS_make return %d, expected 0\n", return_value);
        return;
    }
    // 2. Spawn 10 writers (producers)
    int writers_pid[10];
    Arguments_pipe_packet_writer writers_arguments[10];
    for(int i=0; i<10; i++){
        writers_arguments[i].writer_id = i+1;
        writers_arguments[i].to_send = 10;
        writers_arguments[i].sleep_time = 4;
        writers_arguments[i].sleep_rate = 5;
        writers_arguments[i].pipefd[DSOS_PIPE_RD] = pipefd[DSOS_PIPE_RD];
        writers_arguments[i].pipefd[DSOS_PIPE_WR] = pipefd[DSOS_PIPE_WR];
        writers_pid[i] = last_pid;
        disastrOS_spawn_withfd(test_itest_pipe_packet_writer, &writers_arguments[i]);
        printf(TESTER_INIT_TAG "spawned writer: %d\n", i+1);
    }
    // 3. Spawn 10 readers (consumers)
    int readers_pid[10];
    Arguments_pipe_packet_reader readers_arguments[10];
    for(int i=0; i<10; i++){
        readers_arguments[i].reader_id = i+1;
        readers_arguments[i].sleep_time = 4;
        readers_arguments[i].sleep_rate = 5;
        readers_arguments[i].pipefd[DSOS_PIPE_RD] = pipefd[DSOS_PIPE_RD];
        readers_arguments[i].pipefd[DSOS_PIPE_WR] = pipefd[DSOS_PIPE_WR];
        readers_pid[i] = last_pid;
        disastrOS_spawn_withfd(test_itest_pipe_packet_reader, &readers_arguments[i]);
        printf(TESTER_INIT_TAG "spawned reader: %d\n", i+1);
    }
    // 4. Close descriptors
    return_value = disastrOS_close(pipefd[DSOS_PIPE_RD]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close RD descriptor at the end) return %d\n", return_value);
    }
    return_value = disastrOS_close(pipefd[DSOS_PIPE_WR]);
    if(return_value != DSOS_SUCCESS){
        printf(TESTER_INIT_TAG "disastrOS_close (during close WR descriptor at the end) return %d\n", return_value);
    }
    // 5. Wait for writers
    int pid, writed = 0;
    for(int i=0; i<10; i++){
        pid = disastrOS_wait(writers_pid[i], &return_value);
        if(pid < 0){
            printf(TESTER_INIT_TAG "error in writers wait\n");
            return;
        }
        if(return_value == -1){
            printf(TESTER_INIT_TAG "error in one writer (detected with wait), exit from the test;\n");
            return;
        }
        writed += return_value;
        printf(TESTER_INIT_TAG "exited writer: %d\n", i+1);
    }
    // 6. Wait for readers
    int readed = 0;
    for(int i=0; i<10; i++){
        pid = disastrOS_wait(readers_pid[i], &return_value);
        if(pid < 0){
            printf(TESTER_INIT_TAG "error in readers wait\n");
            return;
        }
        if(return_value == -1){
            printf(TESTER_INIT_TAG "error in one reader (detected with wait), exit from the test;\n");
            return;
        }
        readed += return_value;
        printf(TESTER_INIT_TAG "exited reader: %d\n", i+1);
    }
    // 7. Return
    printf(TESTER_INIT_TAG "the writers sended in total: %d packet.\n", writed);
    printf(TESTER_INIT_TAG "the readers received in total: %d packet.\n", readed);
    printf("----------------------------------------------------------------------------------------------\n");
    return;
}