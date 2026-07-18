# ==========================================
# TICK-TO-TRADE: MOCK EXCHANGE MAKEFILE
# ==========================================

# --- 1. MCU & TOOLCHAIN DEFINITIONS ---
MCU = -mmcu=atmega328p
F_CPU = 16000000UL
TARGET = exchange

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# --- 2. COMPILER FLAGS ---
# -Os: Optimize for size
# -Wall: Enable all compiler warnings
CFLAGS = -Os -Wall $(MCU) -DF_CPU=$(F_CPU)

# --- 3. AVRDUDE (FLASHING) CONFIGURATION ---
PORT = /dev/ttyACM0      # Standard Ubuntu USB port mapping for the Uno
BAUD = 115200            # The Uno bootloader's expected baud rate
PROGRAMMER = arduino     # The protocol ID for avrdude

# --- 4. BUILD TARGETS ---

# 'all' is the default target when you just type 'make'
all: $(TARGET).hex

# Step 1: Compile the C source code into an ELF (Executable and Linkable Format) object
$(TARGET).elf: $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET).elf $(TARGET).c

# Step 2: Strip the heavy ELF file down to a raw Intel HEX file for the flash memory
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

# Step 3: Flash the HEX file to the ATmega328P via USB
flash: $(TARGET).hex
	$(AVRDUDE) -F -V -c $(PROGRAMMER) -p atmega328p -P $(PORT) -b $(BAUD) -U flash:w:$(TARGET).hex:i

# Utility target to clean up compiled files
clean:
	rm -f *.hex *.elf