##
##  Provide by Walon Li
##  2015/05/16
##

CC=gcc
LD=ld

DD=dd
RM=rm
ECHO=echo
CP=cp

all: kernel.bin
	@$(ECHO) "**********Making kernel**********"
	@$(CP) kernel.bin ../

kernel.bin: kernel.s start.c
	# workaround
	# due to I have no idea to resolve elf64, so compile i386 for it.
	$(CC) -m32 -c kernel.s
	$(CC) -m32 -fno-stack-protector -c start.c
	$(LD) -s -Ttext 0x30400 -melf_i386 kernel.o start.o -o kernel.bin

clean:
	@$(RM) -f *.o *.elf *.bin boot.img bochsout.txt
    

