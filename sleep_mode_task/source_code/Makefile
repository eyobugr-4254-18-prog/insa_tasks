#-----------------------------
# Project Settings
#-----------------------------
TARGET = sleep_mode
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = COM8          # Windows (e.g. COM3)
# PORT = /dev/ttyUSB0 # Linux
BAUD = 115200

#-----------------------------
# Tools
#-----------------------------
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

#-----------------------------
# Compiler Options
#-----------------------------
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall

#-----------------------------
# Build Rules
#-----------------------------

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c lcd_test.c 
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) $<

upload: $(TARGET).hex
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$(TARGET).hex:i

clean:
	rm -f *.elf *.hex