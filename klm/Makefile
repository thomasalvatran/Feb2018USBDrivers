# Makefile for stick_driver.c
obj-m := pen_driver.o

KDIR=/usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean # from lkmpg 
	rm -rvf *~
