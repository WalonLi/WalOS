##
##  Provide by Walon Li
##  2015/05/16
##

CC=gcc
LD=ld

#LDFILES_BOOT=walos_x86_boot.ld
#LDFILES_DOS=walos_x86_dos.ld

OBJCPY=objcopy
TRIM_FLAGS=-R .pdr -R .comment -R.note -S -O binary
DD=dd
RM=rm
ECHO=echo
CP=cp

all: kernel.bin
	@$(ECHO) "**********Making kernel**********"
	@$(CP) kernel.bin ../

kernel.bin: kernel.s
	$(CC) -c kernel.s
	$(LD) kernel.o -o kernel.elf
	@$(OBJCPY) $(TRIM_FLAGS) kernel.elf kernel.bin

clean:
	@$(RM) -f *.o *.elf *.bin boot.img bochsout.txt
    

