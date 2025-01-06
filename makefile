CC = /usr/local/i386elfgcc/bin/i386-elf-gcc

LD = /usr/local/i386elfgcc/bin/i386-elf-ld

CFLAGS= -ffreestanding -m32 -g -c

SRC = src

BIN= ./bins

_CSRCS := $(shell find ./ -name "*.cpp")
CSRCS = $(patsubst %.cpp,%,$(_CSRCS))

ASMBINS := src/bootloader/boot src/bootloader/zeroes
ASMELFS := src/kernel/idtasm src/bootloader/kernel_entry

all: prebuild build

run: prebuild build
	qemu-system-x86_64 -drive format=raw,file=$(BIN)/OS.bin,index=0,if=floppy,  -m 128M

build: bin elf c
	$(LD) -o $(BIN)/full_kernel.bin -Ttext 0x1000 $(shell find $(BIN) -name "*.o" | xargs)  --oformat binary
	cat $(BIN)/bootloader/boot.bin $(BIN)/full_kernel.bin $(BIN)/bootloader/zeroes.bin  > $(BIN)/OS.bin

prebuild: 
	rm -rf $(BIN)
	mkdir $(BIN)


elf: $(ASMELFS) 

$(ASMELFS): %: %.asm
	mkdir -p $(BIN)/$(shell dirname $(subst $(SRC)/,,$@))
	nasm $< -f elf -o $(subst $(SRC),$(BIN),$@.o)

bin: $(ASMBINS) 

$(ASMBINS): %: %.asm
	mkdir -p $(BIN)/$(shell dirname $(subst $(SRC)/,,$@))
	nasm $< -f bin -o $(subst $(SRC),$(BIN),$@.bin) 

c: $(CSRCS) 

$(CSRCS): % : %.cpp
	mkdir -p $(BIN)/$(shell dirname $(subst $(SRC)/,,$@))
	$(CC) $(CFLAGS) $< -o $(subst $(SRC),$(BIN),$@.o)

	
.PHONY: clean 

clean: 
	rm -rf $(BIN) 
