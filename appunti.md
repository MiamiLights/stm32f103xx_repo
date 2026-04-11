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

Il *Location Counter* è una speciale variabile interna rappresentata da un singolo punto (.). Agisce come un cursore che tiene traccia dell'indirizzo di memoria corrente mentre il linker svolge il suo lavoro.


### Info sui registri

Flag principali dei registri di stato:

SB si pulisce leggendo da SR1 e poi scrivendo in I2C_DR

ADDR si pulisce leggendo I2C_SR1 e I2C_SR2

TXE si pulisce scrivendo in DR

RXNE si pulisce leggendo in DR

BTF si pulisce leggendo SR1 e leggendo/scrivendo da DR.


### Debug

make load (con openocd)

per connettere gdb a openocd:
 
 arm-none-eabi-gdb -tui (per interfaccia grafica)

gdb: target extended-remote :3333

monitor reset init
monitor reset halt

b (indirizzo o variabile per breakpoint)
display VAR (tiene traccia a ogni iterazione del valore della variabile)



### Clock

Le sorgenti di clock possono essere interne o esterne, si tratta in entrambi i casi di oscillatori al quarzo, entrambe le sorgenti emettono una frequenza specifica che viene moltiplicata attraverso un componente chiamato PLL (phase-locked loop). 
Si può scegliere di usare il clock naturale delle due sorgenti quindi HSI (high speed internal) e HSE (high speed external) o usare la frequenza moltiplicata e quindi PLL.

Per impostare un clock specifico le fasi da seguire sono:
1. scegliere la sorgente di clock, attivarla e attendere
2. impostare la latenza della flash (wait stages)
3. impostare il prescaler del bus (ricorda che apb1 è max 36 mhz)
4. accendere pll e attendere
5. eseguire lo switch finale
