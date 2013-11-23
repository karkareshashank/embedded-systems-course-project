

ifeq ($(ARCH),arm)
        CC = arm-angstrom-linux-gnueabi-gcc
else
        CC = gcc
endif

LIB= -pthread

default: 
	$(CC) $(LIB) -c my_signal_api.c
	$(CC) $(LIB) -o task4_app task4_multiple_signal.c  my_signal_api.o

clean:
	rm -rf *.o task4_app
