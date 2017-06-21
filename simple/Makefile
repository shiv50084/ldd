MODULE_NAME := simple

ifneq ($(KERNELRELEASE),)

obj-m += $(MODULE_NAME).o
ccflags-y := -std=gnu11 -DDEBUG

else

PWD := $(shell pwd)
KDIR ?= /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD)

clean:
	make -C $(KDIR) M=$(PWD) clean

endif
