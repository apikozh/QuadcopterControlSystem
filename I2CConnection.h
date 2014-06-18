#ifndef I2CCONNECTION_H_
#define I2CCONNECTION_H_

#include "types.h"

/*abstract*/ struct I2CConnection {
	
	virtual void setSlaveAddress(uint16 address) = 0;
	virtual int32 getSlaveAddress() = 0;
	
	virtual void writeQuick(uint8 value) = 0;
	virtual uint8 readByte() = 0;
	virtual void writeByte(uint8 value) = 0;

	virtual uint8 readByteData(uint8 command) = 0;
	virtual void writeByteData(uint8 command, uint8 value) = 0;
	virtual uint16 readWordData(uint8 command) = 0;
	virtual void writeWordData(uint8 command, uint16 value) = 0;

	virtual uint8 readBlockData(uint8 command, uint8* values) = 0;
	virtual void writeBlockData(uint8 command, uint8 length, const uint8* values) = 0;
	virtual uint8 readI2CBlockData(uint8 command, uint8 length, uint8* values) = 0;
	virtual void writeI2CBlockData(uint8 command, uint8 length, const uint8* values) = 0;

	virtual uint16 processCall(uint8 command, uint16 value) = 0;
	virtual uint8 blockProcessCall(uint8 command, uint8 length, uint8* values) = 0;
	
	bool readBit(uint8 command, uint8 offset) {
		uint8 res = readByteData(command);
		return res & (1 << offset);
	}

	void writeBit(uint8 command, uint8 offset, bool value) {
		uint8 res = readByteData(command);
		res = value ? res | (1 << offset) : res & ~(1 << offset);
		writeByteData(command, res);
	}

	uint8 readBits(uint8 command, uint8 offset, uint8 length) {
		uint8 res = readByteData(command);
		uint8 mask = (1 << length) - 1;
		return (res >> offset) & mask;
	}

	void writeBits(uint8 command, uint8 offset, uint8 length, uint8 value) {
		uint8 res = readByteData(command);
		uint8 mask = ((1 << length) - 1) << offset;
		value = (value << offset) & mask;
		res &= ~mask;
		res |= value;
		writeByteData(command, res);
	}
	
	/*enum AddressLength {
		SEVENBIT_ADDR,
		TENBIT_ADDR
	};*/
	
//	virtual void setTenbitAddressEnabled(bool value) = 0;
//	virtual bool isTenbitAddressEnabled() = 0;
	
/*
TODO

ioctl(file, I2C_TENBIT, long select)
  Selects ten bit addresses if select not equals 0, selects normal 7 bit
  addresses if select equals 0. Default 0.  This request is only valid
  if the adapter has I2C_FUNC_10BIT_ADDR.

ioctl(file, I2C_PEC, long select)
  Selects SMBus PEC (packet error checking) generation and verification
  if select not equals 0, disables if select equals 0. Default 0.
  Used only for SMBus transactions.  This request only has an effect if the
  the adapter has I2C_FUNC_SMBUS_PEC; it is still safe if not, it just
  doesn't have any effect.

ioctl(file, I2C_FUNCS, unsigned long *funcs)
  Gets the adapter functionality and puts it in *funcs.

ioctl(file, I2C_RDWR, struct i2c_rdwr_ioctl_data *msgset)
  Do combined read/write transaction without stop in between.
  Only valid if the adapter has I2C_FUNC_I2C.  The argument is
  a pointer to a


  */

};

#endif