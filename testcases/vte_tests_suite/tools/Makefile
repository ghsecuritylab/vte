#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the tools test applications.
#
###############################################################################
#Revision History:
#                    Modification     Tracking
#Author                  Date          Number    Description of Changes
#------------------  ------------    ----------  ----------------------
# Spring              15/01/2008        N/A         Initial version
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

SUBDIRS= dumpmem
#Platform identify script
TARGETS  = platfm.sh pkg_handle.sh unit_test_check.sh
TARGETS += get_total_memory.sh v4l_module.sh dump-clocks ipuperf.py clocks.sh
TARGETS += prepare_ubuntu_rfs.sh prepare_offline_vte.sh sd_updater.sh
TARGETS += setup_rootfs.sh 
TARGETS += dt_x86

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@for i in $(TARGETS); do \
		ln -f $$i ../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@echo

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


