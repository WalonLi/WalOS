##
##  Provide by Walon Li
##  2015/05/12
##

CC=gcc
LD=ld

RM=rm
CP=cp
DD=dd
ECHO=echo
MAKE=make
MKDIR=mkdir

SUBDIRS=boot kernel

BUILD=build

OBJCPY=objcopy
TRIM_FLAGS=-R .pdr -R .comment -R.note -S -O binary


# LD
LDFILES_BOOT=boot/walos_x86_boot.ld
LDFILES_DOS=boot/walos_x86_dos.ld

#.PHONY: subdirs $(SUBDIRS)
#subdirs: $(SUBDIRS)

# $< : first prerequisites
# $? : all prerequisites
# $@ : target

all: $(BUILD)/boot.img $(BUILD)/loader.bin
	@$(ECHO) "********** DONE **********"

# boot folder
$(BUILD)/boot.img: $(BUILD)/boot.bin
	@$(ECHO) "**********Making boot.img**********"
	$(DD) if=$< of=$@ bs=512 count=1 conv=notrunc
	$(DD) if=/dev/zero of=$@ skip=1 seek=1 bs=512 count=2879

$(BUILD)/boot.bin: boot/boot.s
	@$(ECHO) "**********Making boot**********"
	@$(RM) -rf $(BUILD)
	@$(MKDIR) $(BUILD)
	$(CC) -Iboot/ -c $< -o $(BUILD)/boot.o
	@$(LD) $(BUILD)/boot.o -o $(BUILD)/boot.elf -e c -T$(LDFILES_BOOT)
	@$(OBJCPY) $(TRIM_FLAGS) $(BUILD)/boot.elf $(BUILD)/boot.bin

$(BUILD)/loader.bin: boot/loader.s
	@$(ECHO) "**********Making loader**********"
	$(CC) -Iboot/ -c $< -o $(BUILD)/loader.o
	@$(LD) $(BUILD)/loader.o -o $(BUILD)/loader.elf -T$(LDFILES_DOS)
	@$(OBJCPY) $(TRIM_FLAGS) $(BUILD)/loader.elf $(BUILD)/loader.bin



#$(SUBDIRS):
#	@$(ECHO) "**********Making***********"
#	$(MAKE) -C $@

copy: $(BUILD)/loader.bin
ifneq ("$(wildcard build/boot.img)","")
	@$(ECHO) "**********Copy $? to image**********"
	@mkdir -p /tmp/floppy;\
	mount -o loop $(BUILD)/boot.img /tmp/floppy/ -o fat=12
	cp $? /tmp/floppy/
	@umount /tmp/floppy/;\
	rm -rf /tmp/floppy/;
else
	@$(ECHO) "*****build/boot.img is not exist *****"
endif

clean:
	$(RM) -f *.o *.elf *.bin boot.img bochsout.txt
	$(RM) -rf build
#	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir clean || exit 1; done




