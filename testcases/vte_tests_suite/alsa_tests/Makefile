#Copyright (C) 2008,2011-2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the ALSA test applications.
#
###############################################################################
#Revision History:
#Author                  Date      Description of Changes
#------------------  ------------  ----------------------
# Spring              07/24/2008   Initial version
# Spring              11/24/2008   Add platform identify script
# Spring              03/21/2011   Add device detection script
# Spring              04/13/2012   Abandon device detection and add default dev
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

SUBDIRS=
SCRIPTS=dac_test1.sh adc_test1.sh dac_vol_adj.sh alsa_ctl.sh dac_channel_swap.sh
SCRIPTS+=speaker_test1.sh speaker_switch_headset.sh
SCRIPTS+=audio_dac_IMX6DL-Sabre-SD.cfg audio_dac_IMX6Solo-SABREAUTO.cfg
SCRIPTS+=audio_dac_IMX6Q-Sabre-SD.cfg audio_dac_IMX6-SABREAUTO.cfg
SCRIPTS+=audio_dac_IMX6-SABRELITE.cfg audio_dac_IMX6Sololite-ARM2.cfg
SCRIPTS+=audio_dac_IMX6ARM2.cfg audio_dac_IMX6DL-ARM2.cfg
SCRIPTS+=audio_dac_IMX6SL-EVK.cfg audio_dac_IMX6DL-SABREAUTO.cfg
SCRIPTS+=audio_dac_IMX53EVK.cfg audio_dac_IMX53SMD.cfg audio_dac_IMX53LOCO.cfg
SCRIPTS+=audio_dac_IMX28EVK.cfg

TARGET=mxc_audio_test.out

CFLAGS+= -Wall -I../../../include -I$(KLINUX_SRCDIR)/include/
LOADLIBES+= -L../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@for i in $(SCRIPTS); do \
		ln -f $$i ../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@-ln -f $(TARGET) ../../bin/
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-rm -f $(TARGET) $(OBJS)
