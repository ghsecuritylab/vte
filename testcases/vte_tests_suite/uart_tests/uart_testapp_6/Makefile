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
#    @file   Makefile
#
#    @brief  configuration file to compile the uart_testapp_6 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#E.Gromazina                  11/04/2005     TLSbo48749   Initial version 
#L.Delaspre/rc149c            14/04/2005     TLSbo48760   VTE 1.9 integration 
#I. Inkina / nknl001          15/06/2005     TLSbo51148   script attached   
#
##################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

CFLAGS+= -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include/
LOADLIBES+=	-L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=uart_testapp_6

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : uart_testapp_6      test application **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	ln -f uart6.sh ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : uart_testapp_6                  **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : uart_test_6.h
