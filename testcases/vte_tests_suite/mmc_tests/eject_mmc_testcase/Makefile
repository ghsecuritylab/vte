#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
#   @file   Makefile
#
#   @brief  configuration file to compile MMC/SD device input/eject testcase
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#S.ZAVJALOV/zvjs001c   22/03/2005   tlsbo46706   Initial version
#L.Delaspre/rc149c     12/04/2005   TLSbo48760   VTE 1.9 integration
#
#
##############################################################################
CFLAGS+=          -Wall -I../../../../include
LOADLIBES+=       -L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=eject_mmc_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : EJECT_MMC_TEST                      **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : EJECT_MMC_TEST                  **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : eject_mmc_test.h
