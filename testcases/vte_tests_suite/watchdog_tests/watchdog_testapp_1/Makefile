#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the watchdog_testapp_1 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V Becker/rc023c              23/07/2004     TLSbo40889  Initial version 
#L Delaspre/rc149c            23/07/2004     TLSbo40891  VTE 1.4 integration 
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS += -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_SRCDIR)/drivers/char \
-I../Kernel_driver
LOADLIBES += -L../../../../lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=watchdog_testapp_1
COMPRESSED_TARGET=watchdog_testapp_1.gz

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)              **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET) $(COMPRESSED_TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : watchdog_test_1.h
