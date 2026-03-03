#
# Makefile - Build bare-metal program for ARM
#
# Usage:
#   make          - Build the program
#   make clean    - Remove build files
#   make install  - Copy to SD card (edit SDCARD_PATH)
#

# Cross compiler
CROSS_COMPILE = arm-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

# Compiler flags
CFLAGS = -mcpu=cortex-a9 -marm -nostdlib -nostartfiles -ffreestanding -Wall -O2

# Target name
TARGET = myprogram

# SD card mount path (change for your system)
SDCARD_PATH = /media/$(USER)/boot

# Source files
SRCS_C = main.c
SRCS_S = start.S

# Object files
OBJS = start.o main.o

#
# Build rules
#

all: $(TARGET).bin
	@echo ""
	@echo "Build complete!"
	@echo "Binary: $(TARGET).bin"
	@$(SIZE) $(TARGET).elf
	@echo ""
	@echo "Usage in U-Boot:"
	@echo "  load mmc 0:1 0x60100000 $(TARGET).bin"
	@echo "  go 0x60100000"

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files
$(TARGET).elf: $(OBJS) linker.ld
	$(LD) -T linker.ld $(OBJS) -o $@

# Create binary from ELF
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Create disassembly (for debugging)
disasm: $(TARGET).elf
	$(OBJDUMP) -d $< > $(TARGET).dis
	@echo "Disassembly saved to $(TARGET).dis"

# Copy to SD card
install: $(TARGET).bin
	@if [ -d "$(SDCARD_PATH)" ]; then \
		cp $(TARGET).bin $(SDCARD_PATH)/; \
		sync; \
		echo "Copied $(TARGET).bin to $(SDCARD_PATH)"; \
	else \
		echo "Error: $(SDCARD_PATH) not found"; \
		echo "Mount your SD card or edit SDCARD_PATH in Makefile"; \
	fi

# Clean build files
clean:
	rm -f *.o *.elf *.bin *.dis

.PHONY: all clean disasm install