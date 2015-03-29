##
##  Provide by Walon Li
##  2015/03/28
##

CC=gcc
LD=ld
LDFILES=walos_x86.ld
OBJCPY=objcopy
DD=dd
RM=rm

all: boot.img

boot.o: boot.S
	$(CC) -c boot.S
    
boot.elf: boot.o
	$(LD) boot.o -o boot.elf -e c -T$(LDFILES)

boot.bin: boot.elf
	@$(OBJCPY) -R .pdr -R .comment -R.note -S -O binary boot.elf boot.bin

boot.img: boot.bin
	@$(DD) if=boot.bin of=boot.img bs=512 count=1 conv=notrunc

clean:
	@$(RM) boot.o boot.elf boot.bin boot.img bochsout.txt
    

