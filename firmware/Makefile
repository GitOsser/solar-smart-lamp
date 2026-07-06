# SON firmware — ATmega2560 (Arduino Mega 2560), bare-metal C/C++
#
#   make            build son-firmware.hex
#   make flash      upload to the board (override PORT=... as needed)
#   make clean      remove build artifacts

MCU        = atmega2560
F_CPU      = 16000000UL
TARGET     = son-firmware

# Upload settings (Arduino Mega 2560 bootloader)
PROGRAMMER = wiring
PORT       = COM3
UPLOAD_BAUD= 115200

CC       = avr-gcc
CXX      = avr-g++
OBJCOPY  = avr-objcopy
SIZE     = avr-size

CPPFLAGS = -I. -DF_CPU=$(F_CPU) -mmcu=$(MCU)
CFLAGS   = -Os -Wall -std=gnu11   -ffunction-sections -fdata-sections
CXXFLAGS = -Os -Wall -std=gnu++17 -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS  = -mmcu=$(MCU) -Wl,--gc-sections

CSRC   := $(wildcard */*.c)
CXXSRC := main.cpp $(wildcard */*.cpp)
OBJ    := $(CSRC:.c=.o) $(CXXSRC:.cpp=.o)

all: $(TARGET).hex

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@
	$(SIZE) --format=avr --mcu=$(MCU) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(TARGET).hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(UPLOAD_BAUD) -D -U flash:w:$<:i

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean
