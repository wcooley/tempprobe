
ifeq ($(KERNELRELEASE),)

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

kernel_objs = 

all: $(kernel_objs)

$(kernel_objs): %.ko: %.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers 

test:
	true

.PHONY: modules modules_install clean test

else
	obj-m := hello_seq.o hello.o sstore.o
endif
