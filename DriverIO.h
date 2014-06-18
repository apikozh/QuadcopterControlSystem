#ifndef DRIVERIO_H_
#define DRIVERIO_H_

#include "types.h"

class DriverIO {

public:
	enum Mode {
		MODE_NONE		= 0,
		MODE_READ		= 1,
		MODE_WRITE		= 2,
		MODE_READWRITE	= 3
	};
	
private:
	int32 file;
	Mode mode;
	
public:
	//DriverIO();
	DriverIO(char* fileName, Mode mode = MODE_READWRITE);
	~DriverIO();
	
	//void write(int8 value);
	//void write(uint8 value);
	//void write(int16 value);
	//void write(uint16 value);
	void write(int32 value);
	void write(uint32 value);
	void write(int64 value);
	void write(uint64 value);
	//void write(float value);
	void write(double value);
	void write(const char* value);

	int32	readInt32();
	uint32	readUInt32();
	int64	readInt64();
	uint64	readUInt64();
	double	readDouble();
	void	read(char* value, int maxLen);
	
};

#endif