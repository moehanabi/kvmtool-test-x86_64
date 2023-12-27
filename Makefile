CFLAGS := -nostdinc -Iinclude -c

NAME    := kernel
BIN     := $(NAME).bin
ELF     := $(NAME).elf
objects := $(patsubst %.c, %.o, $(wildcard *.c)) kernel.o

all: $(BIN)

$(BIN): $(ELF)
	objcopy -O binary $< $@

$(ELF): $(objects)
	ld -T linker.ld -nostdlib -static $(objects) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(ELF) $(objects)

.PHONY: clean
