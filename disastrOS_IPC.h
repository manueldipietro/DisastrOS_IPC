#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"


typedef struct IPC{
    Resource res;
}IPC;

// Bisogna aggiungere allocatore e deallocatore.

int IPC_mk(int resource_id);
int IPC_open(int resource_id, int flags);       // Questa non serve

int IPC_read(int fd, void* buffer, int count);  // Pur non usando void* questo deve essere definito per convenzione con la VMT
int IPC_write(int fd, const void* buffer, int count);

int IPC_close(int fd);                          // Non dovrebbe essere ridefinito, ma bisogna verificare se ci sono comportamenti particolari quando si va a chiudere la risorsa
int IPC_unlink(int resource_id);                // Non serve ridefinirlo
