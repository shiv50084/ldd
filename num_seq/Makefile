MODULE_NAME := num_seq 

ifneq ($(KERNELRELEASE),)

obj-m += $(addsuffix .o,$(MODULE_NAME))
ccflags-y := -std=gnu11 -DDEBUG

else

PWD := $(shell pwd)
KDIR ?= /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=$(PWD)

clean:
	make -C $(KDIR) M=$(PWD) clean

endif
