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
#	@file   Makefile
#
#	@brief  configuration file to compile the oss_audio_play_testapp_0 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
#L.Delaspre/rc149c            09/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            13/12/2004     TLSbo45068  .wav files issues
#D.Khoroshev/B00313           02/10/2006     TLSbo62323  Code cleaning
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS+=	-Wall -I../../../../include  -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/include \
		-I$(KLINUX_SRCDIR)/sound/oss
LOADLIBES+=	-L../../../../lib -lltp

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_MC13783_PMIC
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

AUDIO_DIR = ../audio_test_files

TARGET=oss_audio_play_testapp_0

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) built      **"
	@echo "***********************************************"
	@echo

install:
	cp $(TARGET) ../../../bin
	cp -uf $(AUDIO_DIR)/ringout8k16S.wav ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET) set    **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)          **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) :
