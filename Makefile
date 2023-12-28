CFLAGS := -nostdinc -fno-builtin -mno-sse -Iinclude -c

NAME    := kernel
BIN     := $(NAME).bin
ELF     := $(NAME).elf
objects := $(patsubst %.c, %.o, $(wildcard *.c)) boot.o
libs	:= lib/libstd.a

all: $(BIN)

$(BIN): $(ELF)
	objcopy -O binary $< $@

$(ELF): $(objects) $(libs)
	ld -T linker.ld -nostdlib -static $(objects) $(libs) -o $@

$(libs): just_run
	make -C lib build

just_run:
	@echo

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) $< -o $@

clean:
	-make -C lib/ clean
	rm -f $(BIN) $(ELF) $(objects)

.PHONY: clean
