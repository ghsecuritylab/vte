export ARCH=$(ARCH_CPU)

obj-m += tcrypt.o

modules:
	make -C $(KLINUX_BLTDIR) SUBDIRS=$$PWD modules

modules_install:
	make -C $(KLINUX_BLTDIR) SUBDIRS=$$PWD modules_install

install:
	ln -f  tcrypt.ko ../../../bin/

all: modules modules_install

clean:
	-rm -f *.ko *.o *.mod.c modules.order Module.symvers .*.cmd
	-rm -rf .tmp_versions
