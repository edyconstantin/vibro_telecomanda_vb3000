MCU     = atmega64a
F_CPU   = 16000000UL
TARGET  = vb3000_atmega64a
SRC     = main.c OLED.c Draws.c KBD.c Message.c UART0.c TimerProg.c Globals.c fmt.c font.c
OBJ     = $(SRC:.c=.o)

CC      = avr-gcc
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu99
CFLAGS  += -I. -ffunction-sections -fdata-sections
# ld_atmega64a_rodata_flash.x = copie a avr5.x cu .rodata mutat in FLASH
# (scripturile de serie ale acestui toolchain pun .rodata in RAM)
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections -Wl,--script=$(CURDIR)/ld_atmega64a_rodata_flash.x

all: $(TARGET).hex size

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .text -j .data -O ihex $< $@

size: $(TARGET).elf
	avr-size --format=avr --mcu=$(MCU) $<

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

flash: $(TARGET).hex
	# avrdude -c <programator> -p m64 -U flash:w:$(TARGET).hex
