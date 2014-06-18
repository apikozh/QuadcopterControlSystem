#ifndef LOCALI2CCONNECTION_H_
#define LOCALI2CCONNECTION_H_

#include "I2CConnection.h"

/*abstract*/ class LocalI2CConnection : public I2CConnection {

	int32 devFile;
	int32 slaveAddr;
	
public:
	LocalI2CConnection(char* devFileName);
	~LocalI2CConnection();

	virtual void setSlaveAddress(uint16 address);
	virtual int32 getSlaveAddress();
	
	virtual void writeQuick(uint8 value);
	virtual uint8 readByte();
	virtual void writeByte(uint8 value);

	virtual uint8 readByteData(uint8 command);
	virtual void writeByteData(uint8 command, uint8 value);
	virtual uint16 readWordData(uint8 command);
	virtual void writeWordData(uint8 command, uint16 value);

	virtual uint8 readBlockData(uint8 command, uint8* values);
	virtual void writeBlockData(uint8 command, uint8 length, const uint8* values);
	virtual uint8 readI2CBlockData(uint8 command, uint8 length, uint8* values);
	virtual void writeI2CBlockData(uint8 command, uint8 length, const uint8* values);

	virtual uint16 processCall(uint8 command, uint16 value);
	virtual uint8 blockProcessCall(uint8 command, uint8 length, uint8* values);

};

#endif