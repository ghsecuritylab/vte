#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the keypad_test_script.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#Hake Huang                    24/11/2008       NA          Initial version

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../../
BIN_DIR = $(LTP_DIR)testcases/

TARGET = $(wildcard *.sh)
ifeq ($(ARCH_PLATFORM), $(filter imx233 imx25stack, $(ARCH_PLATFORM)))
TARGET = $(wildcard *.sh)
DATA = keymap_mx233.txt
endif


all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	set -e; for i in $(TARGET); do ln -f $$i $(BIN_DIR)bin; done
	touch $(BIN_DIR)bin/keymap.txt
	ln -f $(DATA) $(BIN_DIR)bin/keymap.txt
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	@echo "do nothing"

