##
##  Provide by Walon Li
##  2015/03/28
##

CC=gcc
LD=ld

LDFILES_BOOT=walos_x86_boot.ld
LDFILES_DOS=walos_x86_dos.ld

OBJCPY=objcopy
TRIM_FLAGS=-R .pdr -R .comment -R.note -S -O binary
DD=dd
RM=rm
ECHO=echo
CP=cp

all: boot.img loader.bin
	@$(ECHO) "**********Making boot**********"
	@$(CP) boot.img loader.bin ../

boot.bin: boot.s
	$(CC) -c boot.s
	$(LD) boot.o -o boot.elf -e c -T$(LDFILES_BOOT)
	@$(OBJCPY) $(TRIM_FLAGS) boot.elf boot.bin

boot.img: boot.bin
	@$(DD) if=boot.bin of=boot.img bs=512 count=1 conv=notrunc
	@$(DD) if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

loader.bin: loader.s
	$(CC) -c loader.s
	$(LD) loader.o -o loader.elf -T$(LDFILES_DOS)
	@$(OBJCPY) $(TRIM_FLAGS) loader.elf loader.bin

clean:
	@$(RM) -f *.o *.elf *.bin boot.img bochsout.txt
    

