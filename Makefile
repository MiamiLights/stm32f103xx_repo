CC=arm-none-eabi-gcc
MACH=cortex-m3
CFLAGS= -c -g -mcpu=$(MACH) -mthumb -mfloat-abi=soft -std=gnu11 -Wall -O0 \
        -Iheaders/core -Iheaders/i2c -Iheaders/uart -Iheaders/DMA
LDFLAGS= -mcpu=$(MACH) -mthumb -mfloat-abi=soft --specs=nano.specs --specs=nosys.specs -T stm32_ls.ld -Wl,-Map=final.map -u _printf_float -lm

SRC_DIR= src
OBJ_DIR= obj

# Trova tutti i file .c in src/ e nelle sue sottocartelle
SRCS= $(shell find $(SRC_DIR) -name "*.c")
# Mappa src/percorso/file.c in obj/file.o
OBJS= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) final.elf

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/core $(OBJ_DIR)/i2c $(OBJ_DIR)/uart $(OBJ_DIR)/DMA

# Regola di compilazione generica per le sottocartelle
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
