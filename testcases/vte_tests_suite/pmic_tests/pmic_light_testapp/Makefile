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
#    @brief  Makefile for PMIC ( SC55112 and MC13783) Light driver test application.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Khoroshev/b00313           07/25/2005     TLSbo52699  Initial version
#D.Khoroshev/b00313           08/29/2005     TLSbo52699  Rework version
#I.Inkina                     28/12/2005     TLSbo61037  update for MXC91231 and MXC91131
#D.Khoroshev/b00313           08/29/2005     TLSbo52699  Update for CO_VTE 2.01
#D.Khoroshev/b00313           08/31/2006     TLSbo76979  Added support for both SC55112 and MC13783
#                                                        platforms
#
####################################################################################################
#Portability: ARM GCC 
####################################################################################################
LTP_DIR=../../../..
BIN_DIR=../../..

CFLAGS+=	-Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include
LOADLIBES+=	-L$(LTP_DIR)/lib -lltp


SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=pmic_light_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "************************************************"
	@echo "** ALL : $(TARGET) test application  **"
	@echo "************************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)/bin
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)               **"
	@echo "************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)                 **"
	@echo "************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : pmic_light_test.h
