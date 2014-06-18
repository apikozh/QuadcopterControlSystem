#ifndef ADXL345_H_
#define ADXL345_H_

#include "AccelSensor.h"
#include "I2CBus.h"
#include "I2CConnection.h"

class ADXL345 : public AccelSensor {
	
public:
	static const uint8 ADDRESS_ALT_LOW		= 0x53; // alt address pin low (GND)
	static const uint8 ADDRESS_ALT_HIGH		= 0x1D; // alt address pin high (VCC)
	static const uint8 DEFAULT_ADDRESS		= ADDRESS_ALT_LOW;

	enum DataRate {
		DATARATE_0_10_HZ = 0x00,
		DATARATE_0_20_HZ = 0x01,
		DATARATE_0_39_HZ = 0x02,
		DATARATE_0_78_HZ = 0x03,
		DATARATE_1_56_HZ = 0x04,
		DATARATE_3_13_HZ = 0x05,
		DATARATE_6_25_HZ  = 0x06,
		DATARATE_12_5_HZ = 0x07,
		DATARATE_25_HZ   = 0x08,
		DATARATE_50_HZ   = 0x09,
		DATARATE_100_HZ  = 0x0A,	// (default)
		DATARATE_200_HZ  = 0x0B,
		DATARATE_400_HZ  = 0x0C,
		DATARATE_800_HZ  = 0x0D,
		DATARATE_1600_HZ = 0x0E,
		DATARATE_3200_HZ = 0x0F
	};

	enum Wakeup {
		WAKEUP_8_HZ	= 0x00,
		WAKEUP_4_HZ	= 0x01,
		WAKEUP_2_HZ	= 0x02,
		WAKEUP_1_HZ	= 0x03
	};

	enum IntPin {
		PIN_INT1 = 0,
		PIN_INT2 = 1
	};

	enum SPIMode {
		SPI_4WIRE = 0,
		SPI_3WIRE = 1
	};
	
	enum IntMode {
		ACTIVE_HIGH = 0,
		ACTIVE_LOW = 1
	};
	
	enum Justify {
		LSB = 0,
		MSB = 1
	};
	
	enum Range {
		RANGE_2G	= 0x00, // +/-  2g (default)
		RANGE_4G	= 0x01, // +/-  4g
		RANGE_8G	= 0x02, // +/-  8g
		RANGE_16G	= 0x03 // +/- 16g
	};
	
	enum FIFOMode {
		FIFO_MODE_BYPASS	= 0x00,
		FIFO_MODE_FIFO		= 0x01,
		FIFO_MODE_STREAM	= 0x02,
		FIFO_MODE_TRIGGER	= 0x03
	};	

private:
	static const uint8 REG_DEVID            = 0x00;	// Device ID
//	static const uint8 REG_RESERVED1        = 0x01;
	static const uint8 REG_THRESH_TAP       = 0x1D;
	static const uint8 REG_OFSX             = 0x1E;
	static const uint8 REG_OFSY             = 0x1F;
	static const uint8 REG_OFSZ             = 0x20;
	static const uint8 REG_DUR              = 0x21;
	static const uint8 REG_LATENT           = 0x22;
	static const uint8 REG_WINDOW           = 0x23;
	static const uint8 REG_THRESH_ACT       = 0x24;
	static const uint8 REG_THRESH_INACT     = 0x25;
	static const uint8 REG_TIME_INACT       = 0x26;
	static const uint8 REG_ACT_INACT_CTL    = 0x27;
	static const uint8 REG_THRESH_FF        = 0x28;
	static const uint8 REG_TIME_FF          = 0x29;
	static const uint8 REG_TAP_AXES         = 0x2A;
	static const uint8 REG_ACT_TAP_STATUS   = 0x2B;
	static const uint8 REG_BW_RATE          = 0x2C;
	static const uint8 REG_POWER_CTL        = 0x2D;	// Power-saving features control
	static const uint8 REG_INT_ENABLE       = 0x2E;
	static const uint8 REG_INT_MAP          = 0x2F;
	static const uint8 REG_INT_SOURCE       = 0x30;
	static const uint8 REG_DATA_FORMAT      = 0x31;
	static const uint8 REG_DATAX0           = 0x32;	// X-axis data 0 (6 bytes for X/Y/Z)
	static const uint8 REG_DATAX1           = 0x33;
	static const uint8 REG_DATAY0           = 0x34;
	static const uint8 REG_DATAY1           = 0x35;
	static const uint8 REG_DATAZ0           = 0x36;
	static const uint8 REG_DATAZ1           = 0x37;
	static const uint8 REG_FIFO_CTL         = 0x38;
	static const uint8 REG_FIFO_STATUS      = 0x39;

	static const uint8 DEVICE_ID			= 0xE5;

	static const uint8 AIC_ACT_AC_BIT		= 7;
	static const uint8 AIC_ACT_X_BIT		= 6;
	static const uint8 AIC_ACT_Y_BIT		= 5;
	static const uint8 AIC_ACT_Z_BIT		= 4;
	static const uint8 AIC_INACT_AC_BIT		= 3;
	static const uint8 AIC_INACT_X_BIT		= 2;
	static const uint8 AIC_INACT_Y_BIT		= 1;
	static const uint8 AIC_INACT_Z_BIT		= 0;

	static const uint8 TAPAXIS_SUP_BIT		= 3;
	static const uint8 TAPAXIS_X_BIT		= 2;
	static const uint8 TAPAXIS_Y_BIT		= 1;
	static const uint8 TAPAXIS_Z_BIT		= 0;

	static const uint8 TAPSTAT_ACTX_BIT		= 6;
	static const uint8 TAPSTAT_ACTY_BIT		= 5;
	static const uint8 TAPSTAT_ACTZ_BIT		= 4;
	static const uint8 TAPSTAT_ASLEEP_BIT	= 3;
	static const uint8 TAPSTAT_TAPX_BIT		= 2;
	static const uint8 TAPSTAT_TAPY_BIT		= 1;
	static const uint8 TAPSTAT_TAPZ_BIT		= 0;

	static const uint8 BW_LOWPOWER_BIT		= 4;
	static const uint8 BW_RATE_BIT 			= 0;
	static const uint8 BW_RATE_LENGTH		= 4;
	
	static const uint8 PCTL_LINK_BIT		= 5;
	static const uint8 PCTL_AUTOSLEEP_BIT	= 4;
	static const uint8 PCTL_MEASURE_BIT		= 3;
	static const uint8 PCTL_SLEEP_BIT		= 2;
	static const uint8 PCTL_WAKEUP_BIT		= 0;
	static const uint8 PCTL_WAKEUP_LENGTH	= 2;

	static const uint8 INT_DATA_READY_BIT	= 7;
	static const uint8 INT_SINGLE_TAP_BIT	= 6;
	static const uint8 INT_DOUBLE_TAP_BIT	= 5;
	static const uint8 INT_ACTIVITY_BIT		= 4;
	static const uint8 INT_INACTIVITY_BIT	= 3;
	static const uint8 INT_FREE_FALL_BIT	= 2;
	static const uint8 INT_WATERMARK_BIT	= 1;
	static const uint8 INT_OVERRUN_BIT		= 0;
	
	static const uint8 FORMAT_SELFTEST_BIT	= 7;
	static const uint8 FORMAT_SPIMODE_BIT	= 6;
	static const uint8 FORMAT_INTMODE_BIT	= 5;
	static const uint8 FORMAT_FULL_RES_BIT	= 3;
	static const uint8 FORMAT_JUSTIFY_BIT	= 2;
	static const uint8 FORMAT_RANGE_BIT		= 0;
	static const uint8 FORMAT_RANGE_LENGTH	= 2;
	
	static const uint8 FIFO_MODE_BIT		= 6;
	static const uint8 FIFO_MODE_LENGTH		= 2;
	static const uint8 FIFO_TRIGGER_BIT		= 5;
	static const uint8 FIFO_SAMPLES_BIT		= 0;
	static const uint8 FIFO_SAMPLES_LENGTH	= 5;

	static const uint8 FIFOSTAT_TRIGGER_BIT	= 7;
	static const uint8 FIFOSTAT_SIZE_BIT	= 0;
	static const uint8 FIFOSTAT_SIZE_LENGTH	= 6;
	
	I2CConnection* bus;
	uint16 address;
	double precision;
	float scaleFactor;
	
public:
	ADXL345(I2CBus* bus, uint16 address = DEFAULT_ADDRESS);
	~ADXL345();
	
	// Device
	virtual void setEnabled(bool value);
	virtual bool isEnabled();
	
	// AccelSensor
	virtual void getLinearAcceleration(double& acc_x, double& acc_y, double& acc_z);
	virtual double getLinearAccelerationRange();
	virtual double getLinearAccelerationPrecision();
	virtual void calibrateAccelerometer();

	// Additional functions
	void calibrate(uint16 samples = 100);
	float getScaleFactor();
	void setScaleFactor(float value);
	
	// DEVID register
	uint8 getDeviceID();

	// THRESH_TAP register
	uint8 getTapThreshold();
	void setTapThreshold(uint8 value);

	// OFS* registers
	void getOffset(int8& x, int8& y, int8& z);
	void setOffset(int8 x, int8 y, int8 z);
	int8 getOffsetX();
	void setOffsetX(int8 value);
	int8 getOffsetY();
	void setOffsetY(int8 value);
	int8 getOffsetZ();
	void setOffsetZ(int8 value);

	// DUR register
	uint8 getTapDuration();
	void setTapDuration(uint8 value);
	
	// LATENT register
	uint8 getDoubleTapLatency();
	void setDoubleTapLatency(uint8 value);

	// WINDOW register
	uint8 getDoubleTapWindow();
	void setDoubleTapWindow(uint8 value);

	// THRESH_ACT register
	uint8 getActivityThreshold();
	void setActivityThreshold(uint8 value);

	// THRESH_INACT register
	uint8 getInactivityThreshold();
	void setInactivityThreshold(uint8 value);

	// TIME_INACT register
	uint8 getInactivityTime();
	void setInactivityTime(uint8 value);

	// ACT_INACT_CTL register
	bool isActivityACEnabled();
	void setActivityACEnabled(bool enabled);
	bool isActivityXEnabled();
	void setActivityXEnabled(bool enabled);
	bool isActivityYEnabled();
	void setActivityYEnabled(bool enabled);
	bool isActivityZEnabled();
	void setActivityZEnabled(bool enabled);
	bool isInactivityACEnabled();
	void setInactivityACEnabled(bool enabled);
	bool isInactivityXEnabled();
	void setInactivityXEnabled(bool enabled);
	bool isInactivityYEnabled();
	void setInactivityYEnabled(bool enabled);
	bool isInactivityZEnabled();
	void setInactivityZEnabled(bool enabled);

	// THRESH_FF register
	uint8 getFreefallThreshold();
	void setFreefallThreshold(uint8 value);

	// TIME_FF register
	uint8 getFreefallTime();
	void setFreefallTime(uint8 value);

	// TAP_AXES register
	bool isTapAxisSuppressEnabled();
	void setTapAxisSuppressEnabled(bool enabled);
	bool isTapAxisXEnabled();
	void setTapAxisXEnabled(bool enabled);
	bool isTapAxisYEnabled();
	void setTapAxisYEnabled(bool enabled);
	bool isTapAxisZEnabled();
	void setTapAxisZEnabled(bool enabled);

	// ACT_TAP_STATUS register
	bool isActivitySourceX();
	bool isActivitySourceY();
	bool isActivitySourceZ();
	bool isAsleep();
	bool isTapSourceX();
	bool isTapSourceY();
	bool isTapSourceZ();

	// BW_RATE register
	bool isLowPowerEnabled();
	void setLowPowerEnabled(bool enabled);
	DataRate getDataRate();
	float getDataRateFreq();
	void setDataRate(DataRate value);

	// POWER_CTL register
	bool isLinkEnabled();
	void setLinkEnabled(bool enabled);
	bool isAutoSleepEnabled();
	void setAutoSleepEnabled(bool enabled);
	bool isMeasureEnabled();
	void setMeasureEnabled(bool enabled);
	bool isSleepEnabled();
	void setSleepEnabled(bool enabled);
	Wakeup getWakeupFrequency();
	void setWakeupFrequency(Wakeup value);
	
	// INT_ENABLE register
	bool isIntDataReadyEnabled();
	void setIntDataReadyEnabled(bool enabled);
	bool isIntSingleTapEnabled();
	void setIntSingleTapEnabled(bool enabled);
	bool isIntDoubleTapEnabled();
	void setIntDoubleTapEnabled(bool enabled);
	bool isIntActivityEnabled();
	void setIntActivityEnabled(bool enabled);
	bool isIntInactivityEnabled();
	void setIntInactivityEnabled(bool enabled);
	bool isIntFreefallEnabled();
	void setIntFreefallEnabled(bool enabled);
	bool isIntWatermarkEnabled();
	void setIntWatermarkEnabled(bool enabled);
	bool isIntOverrunEnabled();
	void setIntOverrunEnabled(bool enabled);

	// INT_MAP register
	IntPin getIntDataReadyPin();
	void setIntDataReadyPin(IntPin pin);
	IntPin getIntSingleTapPin();
	void setIntSingleTapPin(IntPin pin);
	IntPin getIntDoubleTapPin();
	void setIntDoubleTapPin(IntPin pin);
	IntPin getIntActivityPin();
	void setIntActivityPin(IntPin pin);
	IntPin getIntInactivityPin();
	void setIntInactivityPin(IntPin pin);
	IntPin getIntFreefallPin();
	void setIntFreefallPin(IntPin pin);
	IntPin getIntWatermarkPin();
	void setIntWatermarkPin(IntPin pin);
	IntPin getIntOverrunPin();
	void setIntOverrunPin(IntPin pin);

	// INT_SOURCE register
	bool isIntDataReady();
	bool isIntSingleTap();
	bool isIntDoubleTap();
	bool isIntActivity();
	bool isIntInactivity();
	bool isIntFreefall();
	bool isIntWatermark();
	bool isIntOverrun();

	// DATA_FORMAT register
	bool isSelfTestEnabled();
	void setSelfTestEnabled(bool enabled);
	SPIMode getSPIMode();
	void setSPIMode(SPIMode mode);
	IntMode getInterruptMode();
	void setInterruptMode(IntMode mode);
	bool isFullResolutionEnabled();
	void setFullResolutionEnabled(bool enabled);
	Justify getDataJustification();
	void setDataJustification(Justify value);
	Range getRange();
	void setRange(Range value);

	// DATA* registers
	void getAcceleration(int16& x, int16& y, int16& z);
	int16 getAccelerationX();
	int16 getAccelerationY();
	int16 getAccelerationZ();

	// FIFO_CTL register
	FIFOMode getFIFOMode();
	void setFIFOMode(FIFOMode mode);
	IntPin getFIFOTriggerInterruptPin();
	void setFIFOTriggerInterruptPin(IntPin pin);
	uint8 getFIFOSamples();
	void setFIFOSamples(uint8 value);

	// FIFO_STATUS register
	bool isFIFOTriggerOccurred();
	uint8 getFIFOSize();
	
};

#endif //ADXL345