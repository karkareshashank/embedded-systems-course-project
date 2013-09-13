obj-m:= Mydriver.o

KDIR:= /lib/modules/$(shell uname -r)/build
PWD:= $(shell pwd)

APP = gmem_tester

default:
	gcc gmem_tester.c -o $(APP) 	
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
	rm -f gmem_tester
