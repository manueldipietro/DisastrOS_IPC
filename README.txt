--> Capire se il readme può essere fatto attraverso PDF o se conviene farlo in WORD e salvarlo in HTML/MARKUP

Si propone un implementazione di disastrOS in cui si sono sviluppati i seguenti aspetti:
1. Modificato il modulo Resource per renderlo il più vicino possibile alla semantica Posix
   adottata anche da Linux. Questo modulo è pensato per gestire risorse generiche (che dovranno poi
   essere specializzate), poichè non si dispone di un VFS e di un sistema che gestisca i nomi dei
   file si è utilizzato il resource_id come se fosse il "nome" di un file, in particolare si è assunto
   che sopra un certo id quel file fosse di tipo virtuale. Questo poi in fase di sviluppo si è rilevato
   abbastanza inutile se non a scopi di debug per tracciare i descrittori e le risorse in memoria usando
   la funzione disastrOS_print, infatti, si è sviluppato il sistema di rimozione (unlinking) delle risorse
   in modo che una risorsa fosse distrutta solo quando tutti l'avessero chiuso, pertanto la unlinking si limita
   a de-indicizzarla dalla lista e sarà poi la close quando invocata su un descrittore (esplicitamente o tramite
   la chiusura del processo) ad andare a distruggere la risorsa sfruttando la Resource_destroy. Le syscall offerte
   da questo modulo per l'utente sono:
   -disastrOS_mkresource;
   -disastrOS_open;
   -disastrOS_close;
   -disastrOS_unlink;
   -disastrOS_write (non implementata è solo virtuale);
   -disastrOS_read (non implementata è solo virtuale);

2. Ipc

3. PIPE/FIFO

Il lavoro è stato validato andando a creare una suite di test.

--> Scrivere che il sistema è a Byte-stream (e non datagram) e che inizialmente si voleva sviluppare
anche un'interfaccia MQ ma poi ci si è resi conto che aveva funzionalità diverse da replicare, è rimasta
comunque l'implementazione della lista di priorità.

STRUTTURA DEI TEST:
Il sistema di test si basa su due tipi di test diversi:
1. Unity test: testano gli edge cases del programma

Modulo Resource:

Modulo IPC: estende la classe Resource gestendo la sincronizzazione tra lettore e scrittore e distinguendo tra i casi bloccanti e non bloccanti, contiene i seguenti metodi:
1. Ipc_init: inizializza il gestore della memoria di disastrOS;
2. Ipc_alloc: è il costruttore di una risorsa di tipo IPC, alloca la risorsa e chiama Ipc_setter per inizializzarla. 
3. Ipc_setter: si occupa di inizializzare
4. 
5. 
6. 
7. 

STRUTTURA DEL SISTEMA DI TEST:
Per testare approfonditamente le funzioni implementate e i casi limiti, facendo proprio il messaggio del corso "BE EVIL WHEN TESTING" si è implementata una suite di test, che esegue due differenti tipi di test:
-Unit Test: testano le singole funzioni di una System Call, concentrandosi in particolare sui casi limite. Per individuare i casi limite si è partiti dai possibili codici di ritorno Posix delle varie System Call.
-Integration Test: testano dei casi d'uso
All'avvio del sistema vengono eseguiti automaticamente gli unit test, poi viene permesso all'utente di scegliere l'integration test che vuole effettuare. Per il test delle risorse base si è ripreso (cambiando i nomi delle System Call) quello base già presente nel repository del corso. Per gli Integration Test



-exit.c: il file è stato modificato per far rilasciare eventuali risorse e timer immediatamente all'uscita dal processo, altrimenti bisogna aspettare che un processo effettuasse la wait affinchè effettivamente venissero rilasciate le risorse e questo era un comportamento non desiderato.
-