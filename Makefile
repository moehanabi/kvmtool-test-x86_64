CFLAGS := -nostdinc -fno-builtin -Iinclude -c

NAME    := kernel
BIN     := $(NAME).bin
ELF     := $(NAME).elf
libs	:= lib/libstd.a

all: build

build:
	make -C init build
	make -C lib build
	ld -T linker.ld -nostdlib -static init/boot.o init/main.o $(libs) -o $(ELF)
	objcopy -O binary $(ELF) $(BIN)

clean:
	-make -C init/ clean
	-make -C lib/ clean
	rm -f $(BIN) $(ELF) $(objects)

.PHONY: clean
