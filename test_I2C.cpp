//#include <linux/i2c-dev.h>
#include "i2c-dev.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

	int file;
	int adapter_nr = 1; /* probably dynamically determined */
	char filename[20];
	int addr = 0x69; 
	
	snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("Failed to open the bus.\n");
		exit(1);
	}
	    
    unsigned long funcs;
    
	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        printf("Failed to get funcs.\n");
    }else{
		printf("Funcs: %x\n", funcs);
    }
	
	if (ioctl(file,I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
    }
    
	char c;
	int reg, value, res;
	do {
		scanf("%c", &c);
		if (c != 'e') {
			printf("Reg: ");
			scanf("%d", &reg);
		}
		switch (c) {
			case 'w':
				printf("Val: ");
				scanf("%d", &value);
				if (i2c_smbus_write_byte_data(file, reg, value) < 0)
					printf("Error!\n");				
				break;
			case 'W':
				printf("Val: ");
				scanf("%d", &value);
				if (i2c_smbus_write_word_data(file, reg, value) < 0)
					printf("Error!\n");				
				break;
			case 'r':
				res = i2c_smbus_read_byte_data(file, reg);
				if (res < 0) {
					printf("Error!\n");				
				}else{
					printf("Res: %x\n", res);
				}
				break;
			case 'R':
				res = i2c_smbus_read_word_data(file, reg);
				if (res < 0) {
					printf("Error!\n");				
				}else{
					printf("Res: %x\n", res);
				}
				break;
		}
	}while(c != 'e');
    
}