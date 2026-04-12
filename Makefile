CC=arm-none-eabi-gcc
MACH=cortex-m3
CFLAGS= -c -g -mcpu=$(MACH) -mthumb -mfloat-abi=soft -std=gnu11 -Wall -O0 -Iheaders
LDFLAGS= -mcpu=$(MACH) -mthumb -mfloat-abi=soft --specs=nano.specs --specs=nosys.specs -T stm32_ls.ld -Wl,-Map=final.map -u _printf_float -lm

SRC_DIR= src
INC_DIR= headers
OBJ_DIR= obj

SRCS= $(wildcard $(SRC_DIR)/*.c)
# Trasforma src/file.c in obj/file.o
OBJS= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) final.elf

# Crea la directory degli oggetti se non esiste
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Regola di compilazione: mette i .o in OBJ_DIR
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

final.elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ -lm

clean:
	rm -rf $(OBJ_DIR) *.elf *.map

load:
	openocd -f interface/stlink.cfg -f board/stm32f103c8_blue_pill.cfg

flash: final.elf
	openocd -f interface/stlink.cfg -f board/stm32f103c8_blue_pill.cfg -c "program final.elf verify reset exit"

.PHONY: all clean load flash
