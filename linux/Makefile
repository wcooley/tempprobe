#
# Copyright (c) 2010 by Freddy Carl, Wil Cooley and Akbar Saidov.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

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

INCLUDE := -I/lib/modules/`uname -r`/build/include -I/usr/include/asm/mach-default/
CFLAGS  := -O2 -Wall -DMODULE -D__KERNEL__ -DLINUX

all:: ${TARGET}.o

${TARGET}.o: ${TARGET}.c
	$(CC) $(CFLAGS) ${INCLUDE} -c ${TARGET}.c

endif

readtemp: readtemp.c
	gcc -g -o $@ $<

clean::
	$(RM) *.cmd *.o *.ko *.mod.c readtemp Module.symvers modules.order
	$(RM) -R .tmp*

