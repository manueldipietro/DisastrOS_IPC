Di seguito si propone un'implementazione di DisastrOS in cui è stato riorganizzato 
il sistema di gestione delle risorse per renderlo quando più possibile compatibile con la semantica POSIX e
con la programmazione in C OOP. In particolare:
-File resource.c/resource.h: contiene l'implementazione del modulo di gestione delle risorse, inoltre,
utilizzando il resource_id come se fosse il "nome di un file" sono riuscito a gestire le risorse anonime (che hanno la particolarità di non essere trovabili con resource_id).
//--> FAR NOTARE CHE NON SONO INDICIZZATE, MA SONO SEMPRE UNLINKED.

1. Refactor disastrOS_descriptor.c:
   -added: Descriptor_destroy
   -added: Descriptor_make
   -added support for DSOS_O_RDONLY, DSOS_O_WRONLY, DSOS_O_RDWR

2. Refactor disastrOS_resource.c
   -
   -
   -

3. Refactor resource's syscalls

4. 