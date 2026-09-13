#-----------------------------
# Sleep Mode Task - Makefile
#
# This Makefile compiles and uploads the ATmega328P sleep mode
# demonstration program to an Arduino board via UART bootloader.
#
# Usage:
#   make          - Compile the project
#   make upload   - Compile and upload to MCU
#   make clean    - Remove build artifacts
#
# Prerequisites:
#   - avr-gcc compiler suite
#   - avrdude programming tool
#   - Arduino board with ATmega328P
#-----------------------------

#-----------------------------
# Project Settings
#-----------------------------
TARGET = sleep_mode
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = COM8          # Windows (e.g. COM3, COM4, ...)
# PORT = /dev/ttyUSB0 # Linux (uncomment for Linux)
BAUD = 115200

#-----------------------------
# Build Tools
#-----------------------------
CC = avr-gcc              # AVR C Compiler
OBJCOPY = avr-objcopy     # Object copy utility (for hex conversion)
SIZE = avr-size           # Size utility (show program size)
AVRDUDE = avrdude         # Programmer utility

#-----------------------------
# Compiler Options
#-----------------------------
# -mmcu=$(MCU)   : Target MCU
# -DF_CPU=$(F_CPU) : Define CPU frequency for timing
# -Os            : Optimize for size (smaller hex file)
# -Wall          : Enable all warnings
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall

#-----------------------------
# Build Rules
#-----------------------------

# Default target: build the hex file
all: $(TARGET).hex

# Compile source files to ELF executable
$(TARGET).elf: $(TARGET).c lcd_test.c
	$(CC) $(CFLAGS) -o $@ $^

# Convert ELF to hex format and show size
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) $<

# Upload hex file to MCU via UART bootloader
upload: $(TARGET).hex
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$(TARGET).hex:i

# Clean up build artifacts
clean:
	rm -f *.elf *.hex

#-----------------------------
# Phony targets (not actual files)
#-----------------------------
.PHONY: all upload clean
