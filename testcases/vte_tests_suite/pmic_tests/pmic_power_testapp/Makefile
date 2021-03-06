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
#    @brief  Rules for compile the pmic_power_testapp test.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#S.Bezrukov/SBAZR1C           07/20/2005     TLSbo52698  Initial version
#N.Filinov/nfili1c            28/12/2005     TLSbo61037  Rework version
#D.Khoroshev/b00313           07/17/2006     TLSbo64236  Added mc13783 legacy support
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
BIN_DIR = ../../../bin

CFLAGS+= -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
LOADLIBES+=     -L../../../../lib -lltp -lpthread

ifeq ($(ARCH_PLATFORM), mxc91131evb)
CFLAGS+=        -DCONFIG_MXC_PMIC_SC55112
CONFIG_FILES = \
	SC55112_pmic_power.cfg \
	SC55112_err_params.cfg
else
CFLAGS+= -DCONFIG_MXC_MC13783_PMIC
CONFIG_FILES = \
	MC13783_pmic_power.cfg \
	MC13783_err_params.cfg
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=pmic_power_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "****************************************************"
	@echo "** ALL : $(TARGET) test application      **"
	@echo "****************************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	cp -f $(CONFIG_FILES) $(BIN_DIR)
	@echo "****************************************************"
	@echo "** INSTALL : $(TARGET)                   **"
	@echo "****************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "****************************************************"
	@echo "** CLEAN : $(TARGET)                     **"
	@echo "****************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)
