obj-m:= i2c-flash.o

PWD:= $(shell pwd)

# Based on the ARCH specified it will choose either arm compiler or gcc for intel arch
ifeq ($(ARCH),arm)
	CC = arm-angstrom-linux-gnueabi-gcc
	KDIR:= /home/shashank/Desktop/ESP/Lab_project/Angstrom-latest-kernel/Kernel_Source_Trees/Angstrom_Linux/kernel/kernel/
else
	CC = gcc
	KDIR:= /lib/modules/$(shell uname -r)/build
endif

APP = test_app

default:
	$(CC) -o $(APP) $(APP).c
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.ko
	rm -f *.o
	rm -f Module.symvers
	rm -f modules.order
	rm -f *.mod.c
	rm -rf .tmp_versions
	rm -f *.mod.c
	rm -f *.mod.o
	rm -f \.*.cmd
	rm -f Module.markers
	rm -f $(APP)
