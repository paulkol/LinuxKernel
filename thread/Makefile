
ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := threads.o

# ccflags-y += -I$~/Desktop/op/inc

else
# normal makefile

 KDIR ?= /lib/modules/`uname -r`/build
# KDIR ?= ~/kernel/linux-4.12.12

default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
