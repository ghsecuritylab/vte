#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  the third part test suite for storage.
#
##############################################################################

SUBDIRS = mxcfb_test
SCRIPTS = hdmi_test.sh hdmi_stress.sh

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILE)gcc; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-ln -f $(SCRIPTS) ../../bin/

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
