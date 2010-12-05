UNAME := $(shell uname -r)
KERNEL26 := 2.6
KERNELVERSION := $(findstring $(KERNEL26),$(UNAME))

all:: readtemp

ifeq ($(KERNELVERSION),2.6)

obj-m   := usb_tempprobe.o

INCLUDE := -I/usr/include/asm/mach-default/
KDIR    := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd)

all::
	$(MAKE) -C $(KDIR) $(INCLUDE) SUBDIRS=$(PWD) modules

else

TARGET  := sstore
INCLUDE := -I/lib/modules/`uname -r`/build/include -I/usr/include/asm/mach-default/
CFLAGS  := -O2 -Wall -DMODULE -D__KERNEL__ -DLINUX
CC      := gcc

all:: ${TARGET}.o

${TARGET}.o: ${TARGET}.c
	$(CC) $(CFLAGS) ${INCLUDE} -c ${TARGET}.c

endif

readtemp: readtemp.c
	gcc -g -o $@ $<

clean::
	$(RM) .sstore* *.cmd *.o *.ko *.mod.c readtemp Module.symvers modules.order
	$(RM) -R .tmp*

