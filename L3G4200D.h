#ifndef L3G4200D_H_
#define L3G4200D_H_

#include "GyroSensor.h"
#include "I2CBus.h"
#include "I2CConnection.h"

class L3G4200D : public GyroSensor {

public:
	static const uint8 ADDRESS_ALT_LOW		= 0x68; // alt address pin low (GND)
	static const uint8 ADDRESS_ALT_HIGH		= 0x69; // alt address pin high (VCC)
	static const uint8 DEFAULT_ADDRESS		= ADDRESS_ALT_HIGH;

	enum DataRate {
		DATARATE_100_HZ  = 0x00,	// (default)
		DATARATE_200_HZ  = 0x01,
		DATARATE_400_HZ  = 0x02,
		DATARATE_800_HZ  = 0x03
	};
	
	enum Range {
		RANGE_250_DPS	= 0x00,	// +/-  250 dps (default)
		RANGE_500_DPS	= 0x01,	// +/-  500 dps
		RANGE_2000_DPS	= 0x02	// +/-  2000 dps
	};
	
private:
	static const uint8 ADDRESS_AUTOINCREMENT	= 0b10000000;

	static const uint8 REG_DEVID            = 0x0F;	// Device ID
	static const uint8 REG_CTRL1			= 0x20;
	static const uint8 REG_CTRL2			= 0x21;
	static const uint8 REG_CTRL3			= 0x22;
	static const uint8 REG_CTRL4			= 0x23;
	static const uint8 REG_CTRL5			= 0x24;
	static const uint8 REG_REFERENCE		= 0x25;
	static const uint8 REG_OUT_TEMP			= 0x26;
	static const uint8 REG_STATUS			= 0x27;
	static const uint8 REG_OUT_X_L			= 0x28;
	static const uint8 REG_OUT_X_H			= 0x29;
	static const uint8 REG_OUT_Y_L			= 0x2A;
	static const uint8 REG_OUT_Y_H			= 0x2B;
	static const uint8 REG_OUT_Z_L			= 0x2C;
	static const uint8 REG_OUT_Z_H			= 0x2D;
	static const uint8 REG_FIFO_CTRL		= 0x2E;
	static const uint8 REG_FIFO_SRC			= 0x2F;
	static const uint8 REG_INT1_CFG			= 0x30;
	static const uint8 REG_INT1_SRC			= 0x31;
	static const uint8 REG_INT1_THS_XH		= 0x32;
	static const uint8 REG_INT1_THS_XL		= 0x33;
	static const uint8 REG_INT1_THS_YH		= 0x34;
	static const uint8 REG_INT1_THS_YL		= 0x35;
	static const uint8 REG_INT1_THS_ZH		= 0x36;
	static const uint8 REG_INT1_THS_ZL		= 0x37;
	static const uint8 REG_INT1_DURATION	= 0x38;

	static const uint8 DEVICE_ID			= 0xD3;

	static const uint8 CTRL_DATARATE_BIT	= 6;
	static const uint8 CTRL_DATARATE_LENGTH	= 2;
	//static const uint8 CTRL_BANDWIDTH_BIT            5
	//static const uint8 CTRL_BANDWIDTH_LENGTH         2
	static const uint8 CTRL_POWER_BIT		= 3;
	static const uint8 CTRL_ZENABLED_BIT	= 2;
	static const uint8 CTRL_YENABLED_BIT	= 1;
	static const uint8 CTRL_XENABLED_BIT	= 0;
	
	/*
	
	enum Justify {
		LSB = 0,
		MSB = 1
	};
	*/
	//static const uint8 CTRL_BIG_ENDIAN        1
	//static const uint8 CTRL_LITTLE_ENDIAN     0	
	
	static const uint8 CTRL_BDU_BIT				= 7;
	//static const uint8 CTRL_JUSTIFY_BIT           6
	static const uint8 CTRL_RANGE_BIT			= 4;
	static const uint8 CTRL_RANGE_LENGTH		= 2;
	static const uint8 CTRL_SELFTEST_BIT		= 1;
	static const uint8 CTRL_SELFTEST_LENGTH		= 2;
	static const uint8 CTRL_SPIMODE_BIT			= 0;
	
	I2CConnection* bus;
	uint16 address;
	double precision;
	double offsetX, offsetY, offsetZ;
	
public:
	L3G4200D(I2CBus* bus, uint16 address = DEFAULT_ADDRESS);
	~L3G4200D();
	
	// Device
	virtual void setEnabled(bool value);
	virtual bool isEnabled();
	
	// GyroSensor
	virtual void getAngularVelocity(double& rot_x, double& rot_y, double& rot_z);
	virtual double getAngularVelocityRange();
	virtual double getAngularVelocityPrecision();
	virtual void calibrateGyroscope();

	// DEVID register
	uint8 getDeviceID();

	// CTRL_REG1 register
	void setOutputDataRate(DataRate value);
	DataRate getOutputDataRate();
	float getDataRateFreq();
//	void setBandwidthCutOffMode(uint8_t mode);
//	uint8_t getBandwidthCutOffMode();
//	float getBandwidthCutOff();
	void setPowerOnEnabled(bool value);
	bool isPowerOnEnabled();
	void setZEnabled(bool value);
	bool isZEnabled();
	void setYEnabled(bool value);
	bool isYEnabled();
	void setXEnabled(bool value);
	bool isXEnabled();
	
	// CTRL_REG4 register
	void setBlockDataUpdateEnabled(bool value);
	bool getBlockDataUpdateEnabled();
	//...
	void setRange(Range value);
	Range getRange();

	
	// OUT_* registers, read-only
	void getAngularVelocity(int16& x, int16& y, int16& z);
	int16 getAngularVelocityX();
	int16 getAngularVelocityY();
	int16 getAngularVelocityZ();


};


#endif //L3G4200D