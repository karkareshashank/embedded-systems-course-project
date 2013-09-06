obj-m:= Mydriver.o

KDIR:= /lib/modules/$(shell uname -r)/build
PWD:= $(shell pwd)


default:
#	gcc main_1.c -o $(APP) -lpthread	
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

