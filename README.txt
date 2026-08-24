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