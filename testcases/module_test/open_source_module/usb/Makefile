#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

obj-m := usbtest.o
TARGETS = usbtest.ko
all:
	$(MAKE) -C $(KLINUX_BLTDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules

install:
	@echo "****************************************"
	@echo "** INSTALL : usbtest.ko                 **"
	@echo "****************************************"
	ln -f $(TARGETS) ../../../bin/
clean:
	@rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order
	@echo "***********************************************"
	@echo "** CLEAN: UDEV_MODULE                        **"
	@echo "***********************************************"
	@echo


.PHONY: install clean


