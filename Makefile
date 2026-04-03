CC=arm-none-eabi-gcc
MACH=cortex-m3
CFLAGS= -c -g -mcpu=$(MACH) -mthumb -mfloat-abi=soft -std=gnu11 -Wall -O0
LDFLAGS= -mcpu=$(MACH) -mthumb -mfloat-abi=soft --specs=nano.specs --specs=nosys.specs -T stm32_ls.ld -Wl,-Map=final.map -u _printf_float
SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o)


all:final.elf

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

final.elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -rf *.o *.elf *.map

load:
	openocd -f interface/stlink.cfg -f board/stm32f103c8_blue_pill.cfg

flash: final.elf
	openocd -f interface/stlink.cfg -f board/stm32f103c8_blue_pill.cfg -c "program final.elf verify reset exit"
