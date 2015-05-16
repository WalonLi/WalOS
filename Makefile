##
##  Provide by Walon Li
##  2015/05/12
##

RM=rm
CP=cp
ECHO=echo
MAKE=make

SUBDIRS=boot kernel

.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)

#all: boot.img
#	@$(ECHO) "***********************************************************************"
#	@$(ECHO) "213Please run command \"sudo make copy\" to copy loader.bin into boot.img."
#	@$(ECHO) "***********************************************************************"

$(SUBDIRS):
	@$(ECHO) "**********Making***********"
	$(MAKE) -C $@

copy: boot.img boot/loader.bin
	@mkdir -p /tmp/floppy;\
	mount -o loop boot.img /tmp/floppy/ -o fat=12;\
	cp loader.bin /tmp/floppy/;\
	cp kernel.bin /tmp/floppy/;\
	umount /tmp/floppy/;\
	rm -rf /tmp/floppy/;

clean:
	@$(RM) -f *.o *.elf *.bin boot.img bochsout.txt
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir clean || exit 1; done




