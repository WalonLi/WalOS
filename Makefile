##
##  Provide by Walon Li
##  2015/05/12
##

CC = gcc
LD = ld
OBJCPY = objcopy


MAKE = make

SUBDIRS = boot kernel

BUILD = build

TRIM_FLAGS = -R .pdr -R .comment -R.note -S -O binary


# LD
LDFILES_BOOT = boot/walos_x86_boot.ld
LDFILES_DOS = boot/walos_x86_dos.ld


BIN_FILES = $(BUILD)/loader.bin $(BUILD)/kernel.bin


GAS_LIBS_SRC = lib/string.s lib/common.s
C_LIBS_SRC =

GAS_LIBS_OBJ = $(subst lib,build,$(GAS_LIBS_SRC:.s=.o))
C_LIBS_OBJ = $(subst lib,build,$(C_LIBS_SRC:.s=.o))

C_FLAGS = -std=c99 -fno-builtin -fno-stack-protector


#.PHONY: subdirs $(SUBDIRS)
#subdirs: $(SUBDIRS)

# $< : first prerequisites
# $? : all prerequisites
# $@ : target

all: $(BUILD)/boot.img $(BIN_FILES)
	@echo "********** DONE **********"

# boot folder
$(BUILD)/boot.img: $(BUILD)/boot.bin
	@echo "**********Making boot.img**********"
	@dd if=$< of=$@ bs=512 count=1 conv=notrunc
	@dd if=/dev/zero of=$@ skip=1 seek=1 bs=512 count=2879

$(BUILD)/boot.bin: boot/boot.s
	@echo "**********Making boot**********"
	@rm -rf $(BUILD)
	@mkdir $(BUILD)
	$(CC) -Iboot/ -c $< -o $(BUILD)/boot.o
	@$(LD) $(BUILD)/boot.o -o $(BUILD)/boot.elf -e c -T$(LDFILES_BOOT)
	@$(OBJCPY) $(TRIM_FLAGS) $(BUILD)/boot.elf $@

$(BUILD)/loader.bin: boot/loader.s
	@echo "**********Making loader**********"
	$(CC) -Iboot/ -c $< -o $(BUILD)/loader.o
	@$(LD) $(BUILD)/loader.o -o $(BUILD)/loader.elf -T$(LDFILES_DOS)
	@$(OBJCPY) $(TRIM_FLAGS) $(BUILD)/loader.elf $@


$(BUILD)/kernel.bin: kernel/kernel.s kernel/start.c $(GAS_LIBS_SRC) $(C_LIBS_SRC)
	@echo "**********Making kernel**********"
# workaround
# due to I have no idea to resolve elf64, so compile i386 for it.
	$(foreach i, $(GAS_LIBS_SRC), $(CC) -m32 -c $(i) -o $(subst lib,build,$(i:.s=.o));)
	$(foreach i, $(C_LIBS_SRC), $(CC) -m32 $(C_FLAGS) -c $(i) -o $(subst lib,build,$(i:.s=.o));) 
#	for i in $(GAS_LIBS_SRC); do \
#		$(CC) -m32 -c $$i -o $(patsubst %.s,%.o, $$i) ; 
#	done

	$(CC) -m32 -c kernel/kernel.s -o $(BUILD)/kernel.o
	$(CC) -m32 $(C_FLAGS) -Iinclude/ -c kernel/start.c -o $(BUILD)/start.o
	$(LD) -s -Ttext 0x30400 -melf_i386 $(BUILD)/kernel.o $(BUILD)/start.o $(GAS_LIBS_OBJ) $(C_LIBS_OBJ) -o $@



#$(SUBDIRS):
#	@$(ECHO) "**********Making***********"
#	$(MAKE) -C $@

copy: $(BIN_FILES)
ifneq ("$(wildcard build/boot.img)","")
	@echo "**********Copy $? to image**********"
	@mkdir -p /tmp/floppy;\
	mount -o loop $(BUILD)/boot.img /tmp/floppy/ -o fat=12
	cp $? /tmp/floppy/
	@umount /tmp/floppy/;\
	rm -rf /tmp/floppy/;
else
	@echo "*****build/boot.img is not exist *****"
endif

clean:
	rm -f *.o *.elf *.bin boot.img bochsout.txt
	rm -rf build
#	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir clean || exit 1; done




