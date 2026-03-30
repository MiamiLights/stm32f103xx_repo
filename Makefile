CC=arm-none-eabi-gcc
MACH=cortex-m3
CFLAGS= -c -mcpu=$(MACH) -mthumb -std=gnu11 -Wall -O0

all: main.o stm32_startup.o

main.o:main.c
	$(CC) $(CFLAGS) $^ -o $@

stm32_startup.o: stm32_startup.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o *.elf
