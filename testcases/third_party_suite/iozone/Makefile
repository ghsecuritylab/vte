###############################################################################
#
#    @file   Makefile
#
#    @brief  iozone
#
##############################################################################

SUBDIRS = iozone3_308/src/current

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) linux-arm -C $$i CC=$(CROSS_COMPILE)gcc GCC=$(CROSS_COMPILE)gcc; done

install:
	-ln -f iozone3_308/src/current/iozone ../../bin/ 
	-ln -f iozone3_308/src/current/fileop ../../bin/
clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
