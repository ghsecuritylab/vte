##############################################################################
#
#    @file   Makefile
#
#    @brief  nettools.
#
##############################################################################

include ../vte_configs/$(ARCH_PLATFORM)_tp_configs

SUBDIRS-$(CONFIG_TP_ETHTOOL) += ethtool-6
clean_SUBDIR-$(CONFIG_TP_ETHTOOL) += clean_ethtool-6
SUBDIRS-$(CONFIG_TP_PPP)     += ppp-2.4.3
clean_SUBDIR-$(CONFIG_TP_PPP) += clean_ppp-2.4.3
SUBDIRS-$(CONFIG_TP_NETPERF) +=  netperf-2.5.0
clean_SUBDIR-$(CONFIG_TP_NETPERF) += clean_netperf-2.5.0
SUBDIRS-$(CONFIG_TP_TCPDUMP) += tcpdump-3.9.8
clean_SUBDIR-$(CONFIG_TP_TCPDUMP) += clean_tcpdump-3.9.8
SUBDIRS-$(CONFIG_TP_MII)     += mii-tool-1.9.1.1
clean_SUBDIR-$(CONFIG_TP_MII) += clean_mii-tool-1.9.1.1
SUBDIRS-$(CONFIG_TP_IPROUTE)     += iproute-20110315
clean_SUBDIR-$(CONFIG_TP_IPROUTE) += iproute-20110315
SUBDIRS-$(CONFIG_TP_NETTOOL) += net-tools-1.60 
clean_SUBDIR-$(CONFIG_TP_NETTOOL) += net-tools-1.60

all: $(SUBDIRS-y)

install:
	-ln -f ethtool-6/ethtool ../../bin/
	-ln -f netperf-2.5.0/src/netperf ../../bin/
	-ln -f netperf-2.5.0/src/netserver ../../bin/
	-ln -f mii-tool-1.9.1.1/mii-tool ../../bin/
	-ln -f ppp-2.4.3/pppd/pppd ../../bin/
	-ln -f tcpdump-3.9.8/tcpdump ../../bin/
	-ln -f iproute-20110315/ip/ip ../../bin/ip
	-ln -f net-tools-1.60/arp ../../bin/
	-ln -f net-tools-1.60/nameif ../../bin/
	-ln -f net-tools-1.60/netstat ../../bin/
	-ln -f net-tools-1.60/rarp ../../bin/
	-ln -f net-tools-1.60/route ../../bin/
	-ln -f net-tools-1.60/slattach ../../bin/

clean: $(clean_SUBDIR-y)

ethtool-6: blank
	cd $@;./myconfig;$(MAKE) CC=$(CROSS_COMPILE)gcc

clean_ethtool-6:
	-$(MAKE) -C ethtool-6 clean; ac_cv_func_setpgrp_void=yes $(MAKE) -C ethtool-6 distclean

ppp-2.4.3: blank
	cd $@;./myconfig; $(MAKE) CC=$(CROSS_COMPILE)gcc

clean_ppp-2.4.3:
	-$(MAKE) -C ppp-2.4.3 clean
	-$(MAKE) -C ppp-2.4.3 dist-clean

netperf-2.5.0: blank
	cd $@; ./myconfig; $(MAKE) CC=$(CROSS_COMPILE)gcc

clean_netperf-2.5.0:
	-$(MAKE) -C netperf-2.5.0 clean; $(MAKE) -C netperf-2.5.0 distclean

tcpdump-3.9.8: libs
	cd $@; ./myconfig; $(MAKE)

clean_tcpdump-3.9.8:
	-$(MAKE) -C  tcpdump-3.9.8 clean
	-$(MAKE) -C  tcpdump-3.9.8 distclean
	$(MAKE) -C $$(pwd)/../../../openlibs clean

mii-tool-1.9.1.1: blank
	$(MAKE) -C $@ CC=$(CROSS_COMPILE)gcc

clean_mii-tool-1.9.1.1:
	-$(MAKE) -C mii-tool-1.9.1.1 clean
	-$(MAKE) -C mii-tool-1.9.1.1 distclean

iproute-20110315: blank
	$(MAKE) -C $@ CC=$(CROSS_COMPILE)gcc

clean_iproute-20110315:
	$(MAKE) -C iproute-20110315 distclean

net-tools-1.60: blank
	$(MAKE) -C  $@ CC=$(CROSS_COMPILE)gcc	

clean_net-tools-1.60:
	$(MAKE) -C  net-tools-1.60 clean	
	
libs:
	$(MAKE) -C $$(pwd)/../../../openlibs

blank:
	echo "start"
