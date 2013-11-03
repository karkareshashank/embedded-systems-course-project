
CC = arm-angstrom-linux-gnueabi-gcc

all: test_app

test_app: test_app.o eeprom.o
	$(CC) test_app.o eeprom.o -o test_app

test_app.o: test_app.c
	$(CC) -c test_app.c

eeprom.o: eeprom.c
	$(CC) -c eeprom.c

clean:
	rm -rf *.o test_app
