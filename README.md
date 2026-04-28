# STM32F103xx Firmware Project

Progetto di firmware per microcontrollori STM32F103xx (serie Blue Pill) sviluppato in C. Il progetto include driver per vari sensori, moduli di comunicazione e gestione periferiche tramite DMA, I2C e UART.

## Funzionalità principali

Il progetto integra il supporto per:
- **Display OLED SSD1306**: Gestione tramite protocollo I2C.
- **Sensore AHT20**: Lettura di temperatura e umidità via I2C.
- **GPS NEO-6M**: Acquisizione dati di posizionamento via UART.
- **Modulo GSM/LTE A7670**: Gestione connettività cellulare via UART.
- **Audio/Buzzer**: Riproduzione di toni (es. Megalovania) tramite DMA.
- **Core**: Startup custom, syscalls e configurazione dei clock di sistema.
- **SPI**: Driver generici per protocollo SPI e implementazione di drivers per scheda SDHC

## Struttura del Progetto

```text
├── headers/         # Header files (.h) suddivisi per modulo (core, DMA, i2c, uart)
├── src/             # File sorgente (.c) organizzati per funzionalità
├── obj/             # File oggetto generati durante la compilazione
├── Makefile         # Script di automazione per la compilazione
├── stm32_ls.ld      # Linker script per STM32F103
└── appunti.md       # Note di sviluppo e documentazione personale
```

## Requisiti

- **Toolchain**: `arm-none-eabi-gcc` (GNU Arm Embedded Toolchain)
- **Build System**: `make`
- **Flash/Debug**: `st-link` o `openocd` (opzionale, per il caricamento sulla scheda)

## Compilazione

Per compilare il progetto e generare il file binario, esegui semplicemente:

```bash
make
```

I file compilati verranno inseriti nella cartella `obj/` e verrà prodotto il file eseguibile `final.elf` .

## Note
Questo progetto non utilizza librerie HAL o standard esterne pesanti, puntando su una gestione diretta dei registri e driver ottimizzati per le periferiche specifiche.
