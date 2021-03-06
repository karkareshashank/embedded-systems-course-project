#ifndef __eeprom_H__
#define __eeprom_H__

#define FILENAME 	"/dev/i2c-2"
#define DEV_ADDR 	0x52
#define PAGE_NUM	512
#define PAGE_SIZE	64



int  read_EEPROM(void* buf,int count);
int  write_EEPROM(const void* buf,int count);
int  seek_EEPROM(int offset);

void change_addr(int offset,int flag);


#endif
