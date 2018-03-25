
NAME:=clock
PROGRAMMER:=usbtiny
PART_SHORT:=t44
PART_LONG:=attiny44a

SRC_DIR:=src
BUILD_DIR:=build
ELF_FILE:=$(BUILD_DIR)/$(NAME).elf
HEX_FILE:=$(BUILD_DIR)/$(NAME).hex

CC:=avr-gcc
CFLAGS:= -std=c11 -Os -DF_CPU=1000000 -mmcu=$(PART_LONG)
LINKER:=avr-gcc
LDFLAGS:= -mmcu=$(PART_LONG)
OBJCOPY:=avr-objcopy

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)


.PHONY: install fuses clean


$(HEX_FILE): $(ELF_FILE) | $(BUILD_DIR)
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(ELF_FILE): $(OBJECTS) | $(BUILD_DIR)
	$(LINKER) $(LDFLAGS) $(OBJECTS) -o $@

# http://stackoverflow.com/a/2501673
DEPS=$(OBJECTS:%.o=%.d)
-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -MMD -MF $(patsubst %.o,%.d,$@) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


install: $(HEX_FILE)
	avrdude -c $(PROGRAMMER) -p $(PART_SHORT) -U flash:w:$<:i

fuses:
	avrdude -F -V -c $(PROGRAMMER) -p $(PART_SHORT) -U lfuse:w:0x7d:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

clean:
	rm -rf $(BUILD_DIR)


