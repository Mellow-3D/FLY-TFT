KDIR ?= /lib/modules/`uname -r`/build

obj-m += st7796.o

all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

