
RISCVGNU = riscv32-none-elf

#MARCH = -march=rv32i -mabi=ilp32
MARCH = -march=rv32ic -mabi=ilp32
#MARCH = -march=rv32imc -mabi=ilp32

AOPS = --warn $(MARCH)
COPS = -Wall $(MARCH) -O2 -nostdlib -nostartfiles -ffreestanding 

all : notmain.bin

clean :
	rm -f *.o
	rm -f *.elf
	rm -f *.bin
	rm -f *.list

novectors.o : novectors.s
	$(RISCVGNU)-as $(AOPS) novectors.s -o novectors.o

notmain.o : notmain.c
	$(RISCVGNU)-gcc $(COPS) -c notmain.c -o notmain.o

notmain.bin : memmap novectors.o notmain.o 
	$(RISCVGNU)-ld novectors.o notmain.o -T memmap -o notmain.elf
	$(RISCVGNU)-objdump -D notmain.elf > notmain.list
	$(RISCVGNU)-objcopy notmain.elf -O binary notmain.bin 

