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
#    @brief  configuration file to compile the GPIO dio_sparse test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# Igor Semenchukov            10/06/2004     TLSbo39741   Initial version 
# Ludovic Delaspre / rc149c   22/06/2004     TLSbo39941   VTE 1.3 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

CFLAGS+=	-Wall -I../../../../include -D_GNU_SOURCE
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=dio_sparse_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : dio_sparse test application         **"
	@echo "***********************************************"
	@echo

install:
	@echo "***********************************************"
	@echo "** INSTALL : dio_sparse_test                 **"
	@echo "***********************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : dio_sparse_test.h



