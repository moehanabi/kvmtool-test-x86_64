NAME	:= kernel

BIN	:= $(NAME).bin
ELF	:= $(NAME).elf
OBJ	:= $(NAME).o

all: $(BIN)

$(BIN): $(ELF)
	objcopy -O binary $< $@

$(ELF): $(OBJ)
	ld -T linker.ld -nostdlib -static $< -o $@

%.o: %.S
	gcc -nostdinc -Iinclude -c $< -o $@

clean:
	rm -f $(BIN) $(ELF) $(OBJ)
.PHONY: clean
