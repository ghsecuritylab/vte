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
#    @brief  configuration file to compile the mu_api_rwstress_test test that performs stress
#            tests of Messaging Unit driver read() and write() system calls
#
###################################################################################################
#Revision History:
#                              Modification     Tracking
#Author (Core ID)                  Date          Number    Description of Changes
#---------------------------   ------------    ----------  ------------------------------------------
#Igor Semenchukov (smng001c)    24/08/2004     TLSbo40411   Initial version 
#L.Delaspre/rc149c              08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

CFLAGS+=	-Wall -I../../../../include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=mu_api_rwstress_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL :   mu_api_rwstress_test application  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL :   mu_api_rwstress_test          **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : mu_api_rwstress_test.h
