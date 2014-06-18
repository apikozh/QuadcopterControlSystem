#include "LocalI2CConnection.h"

#include "i2c-dev.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

LocalI2CConnection::LocalI2CConnection(char* devFileName) {
	devFile = open(devFileName, O_RDWR);
	if (devFile < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("Failed to open the bus.\n");
		exit(1);
	}
	slaveAddr = -1;
}

LocalI2CConnection::~LocalI2CConnection() {
	close(devFile);
}

void LocalI2CConnection::setSlaveAddress(uint16 address) {
	if (slaveAddr != address) {
		slaveAddr = address;
		
		if (ioctl(devFile, I2C_SLAVE, slaveAddr) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
			exit(1);
		}
	}
}

int32 LocalI2CConnection::getSlaveAddress() {
	return slaveAddr;
}

void LocalI2CConnection::writeQuick(uint8 value) {
	int32 result = i2c_smbus_write_quick(devFile, value);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
}

uint8 LocalI2CConnection::readByte() {
	int32 result = i2c_smbus_read_byte(devFile);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}

void LocalI2CConnection::writeByte(uint8 value) {
	int32 result = i2c_smbus_write_byte(devFile, value);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
}

uint8 LocalI2CConnection::readByteData(uint8 command) {
	int32 result = i2c_smbus_read_byte_data(devFile, command);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}

void LocalI2CConnection::writeByteData(uint8 command, uint8 value) {
	int32 result = i2c_smbus_write_byte_data(devFile, command, value);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
}

uint16 LocalI2CConnection::readWordData(uint8 command) {
	int32 result = i2c_smbus_read_word_data(devFile, command);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}

void LocalI2CConnection::writeWordData(uint8 command, uint16 value) {
	int32 result = i2c_smbus_write_word_data(devFile, command, value);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
}

uint8 LocalI2CConnection::readBlockData(uint8 command, uint8* values) {
	int32 result = i2c_smbus_read_block_data(devFile, command, values);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}

void LocalI2CConnection::writeBlockData(uint8 command, uint8 length, const uint8* values) {
	int32 result = i2c_smbus_write_block_data(devFile, command, length, values);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
}

uint8 LocalI2CConnection::readI2CBlockData(uint8 command, uint8 length, uint8* values) {
	int32 result = i2c_smbus_read_i2c_block_data(devFile, command, length, values);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}

void LocalI2CConnection::writeI2CBlockData(uint8 command, uint8 length, const uint8* values) {
	int32 result = i2c_smbus_write_i2c_block_data(devFile, command, length, values);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
}
	
uint16 LocalI2CConnection::processCall(uint8 command, uint16 value) {
	int32 result = i2c_smbus_process_call(devFile, command, value);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}

uint8 LocalI2CConnection::blockProcessCall(uint8 command, uint8 length, uint8* values) {
	int32 result = i2c_smbus_block_process_call(devFile, command, length, values);
	if (result < 0) {
		printf("i2c transaction failed.\n");
		exit(1);
	}
	return result;
}
