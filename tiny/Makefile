KDIR ?= /lib/modules/`uname -r`/build

obj-m += st7796.o

default:
	$(MAKE) -C $(KDIR) M=$$PWD
install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

