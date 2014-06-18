#include "LocalI2CBus.h"

#include "i2c-dev.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

LocalI2CBus::LocalI2CBus(uint8 busNumber) {
	this->busNumber = busNumber;
	snprintf(devFileName, 19, "/dev/i2c-%d", busNumber);
	if (access(devFileName, F_OK) != 0) {
		printf("Failed to find the bus.\n");
		exit(1);
	}
}

LocalI2CConnection* LocalI2CBus::createConnection() {
	return new LocalI2CConnection(devFileName);
}

uint8 LocalI2CBus::getBusNumber() {

}

char* LocalI2CBus::getDeviceFileName() {

}
