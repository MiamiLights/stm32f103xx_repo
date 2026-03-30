### Linker Script
Il Linker é un componente di una toolchain e ha lo scopo di combinare molteplici file oggetto in file eseguibili. Assolve a compiti critici quali: 
- Risoluzione dei Simboli (unire i puntini)
- Combinazione file oggetto (accorpare gli oggetti)
- Rilocazione (assegnare indirizzi fisici)
- Library Handling

Il Linker Script permette di personalizzare come queste operazioni devono essere effettuate, specialmente in ambito embedded dove l'architettura cambia e le specifiche del sistema non sono note alla toolchain.

Le componenti di un linker script sono le seguenti:

- ENTRY, definisce qual é il punto esatto in cui inizia un programma.
- MEMORY, definisce quali sono le aree di memoria disponibili e quanto sono grandi.
- SECTIONS, definisce come accorpare le parti dei file .o generate dal compilatore. La decisione della posizione del segmento avviene tramite il comando >vma (virtual memory address) AT> lma (load memory address). VMA é l'indirizzo dove la sezione esisterà e sarà acceduta dalla CPU a runtime, mentre LMA é l'indirizzo dove la sezione sarà fisicamente contenuta o caricata.
