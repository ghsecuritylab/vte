SUBDIRS += libpcap-0.9.6 libpng-1.6.2 libjpeg6b-6b1
export CFLAGS +=  -I$(KLINUX_SRCDIR)/include

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILE)gcc; done
	$(MAKE) -C libpcap-0.9.6  CC=$(CROSS_COMPILE)gcc shared
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	$(MAKE) -C libpcap-0.9.6 install-shared
install:
	echo "usually the rootfs will have test libs"
clean:
	-@set -e; for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(SUBDIRS) ; do $(MAKE) -C $$i distclean ; done
	-$(MAKE) -C zlib-1.2.3 clean
	-$(MAKE) -C zlib-1.2.3 distclean
	rm -rf bin lib share man include
config:
	cd zlib-1.2.3; ./myconfig ; cd ..
	make -C zlib-1.2.3 CC=$(CROSS_COMPILE)gcc
	-make -C zlib-1.2.3 install
	@set -e; for i in $(SUBDIRS) ; do cd $$i ; ./myconfig ; cd ..; done

