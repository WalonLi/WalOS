##
##  Provide by Walon Li
##  2015/03/28
##

CC=gcc
LD=ld

LDFILES_BOOT=walos_x86.ld
LDFILES_DOS=walos_x86_dos.ld

OBJCPY=objcopy
TRIM_FLAGS=-R .pdr -R .comment -R.note -S -O binary
DD=dd
RM=rm
ECHO=echo

all: boot.img loader.bin
	@$(ECHO) "***********************************************************************"
	@$(ECHO) "Please run command \"sudo make copy\" to copy loader.bin into boot.img."
	@$(ECHO) "***********************************************************************"
boot.bin: boot.s
	$(CC) -c boot.s
	$(LD) boot.o -o boot.elf -e c -T$(LDFILES_BOOT)
	@$(OBJCPY) $(TRIM_FLAGS) boot.elf boot.bin

loader.bin: loader.s
	$(CC) -c loader.s
	$(LD) loader.o -o loader.elf -T$(LDFILES_DOS)
	@$(OBJCPY) $(TRIM_FLAGS) loader.elf loader.bin

boot.img: boot.bin
	@$(DD) if=boot.bin of=boot.img bs=512 count=1 conv=notrunc
	@$(DD) if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

copy: boot.img loader.bin
	@mkdir -p /tmp/floppy;\
	mount -o loop boot.img /tmp/floppy/ -o fat=12;\
	cp loader.bin /tmp/floppy/;\
	umount /tmp/floppy/;\
	rm -rf /tmp/floppy/;

clean:
	@$(RM) -f *.o *.elf *.bin boot.img bochsout.txt
    

