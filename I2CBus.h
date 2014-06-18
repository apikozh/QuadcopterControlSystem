#ifndef I2CBUS_H_
#define I2CBUS_H_

#include "I2CConnection.h"

/*abstract*/ struct I2CBus {
	
	virtual I2CConnection* createConnection() = 0;

};

#endif