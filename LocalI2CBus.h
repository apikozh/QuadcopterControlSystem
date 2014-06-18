#ifndef LOCALI2CBUS_H_
#define LOCALI2CBUS_H_

#include "I2CBus.h"
#include "LocalI2CConnection.h"

class LocalI2CBus : public I2CBus {

	uint8 busNumber;
	char devFileName[20];
	
public:
	LocalI2CBus(uint8 busNumber);

	virtual LocalI2CConnection* createConnection();
	
	uint8 getBusNumber();
	char* getDeviceFileName();
		
};

#endif