#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the scc module.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Urusov/NONE                26/09/2005     TLSbo55835  Initial version
#
####################################################################################################
EXTRA_CFLAGS += -I$(KLINUX_BLTDIR)/include \
          -I$(KLINUX_BLTDIR)/arch/arm/plat-mxc/include \
	  -I$(KLINUX_BLTDIR)/include \
          -DSCC_MODULE \
          -DSCC_DEBUG  \
          -DIMX_NEEDS_DEPRECATED_SYMBOLS

LOADLIBES =

TARGET = scc_test_module.ko
obj-m= scc_test_module.o

all:
	@make -C $(KLINUX_BLTDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : SCC_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f scc_test_module.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions modules.order
	@echo "***********************************************"
	@echo "** CLEAN: SCC_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : SCC_TEST_MODULE                 **"
	@echo "***********************************************"
	@echo
