#include "ADXL345.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

ADXL345::ADXL345(I2CBus* _bus, uint16 address) : bus(_bus->createConnection()) {
	this->address = address;
	bus->setSlaveAddress(address);
	if (getDeviceID() != DEVICE_ID) {
		printf("Device ID is not match for ADXL345.\n");
		exit(1);
	}
	precision = 0;
	scaleFactor = 1;
}

ADXL345::~ADXL345() {
	delete bus;
}

void ADXL345::setEnabled(bool value) {
	setMeasureEnabled(value);
}

bool ADXL345::isEnabled() {
	return isMeasureEnabled();
}

void ADXL345::getLinearAcceleration(double& acc_x, double& acc_y, double& acc_z) {
	int16 x, y, z;
	getAcceleration(x, y, z);
	if (!precision)
		precision = getLinearAccelerationPrecision();
	acc_x = x * precision;
	acc_y = y * precision;
	acc_z = z * precision;
}

double ADXL345::getLinearAccelerationRange() {
	Range range = getRange();
	switch (range) {
		case RANGE_2G:	return (double)scaleFactor * 2 * 9.81;
		case RANGE_4G:	return (double)scaleFactor * 4 * 9.81;
		case RANGE_8G:	return (double)scaleFactor * 8 * 9.81;
		default: 		return (double)scaleFactor * 16 * 9.81;
	}
}

double ADXL345::getLinearAccelerationPrecision() {
	Range range = getRange();
	if (RANGE_2G == range || isFullResolutionEnabled()) {
		return (double)scaleFactor * 4 * 9.81 / 1024;
	}else if (RANGE_4G == range){
		return (double)scaleFactor * 8 * 9.81 / 1024;
	}else if (RANGE_8G == range){
		return (double)scaleFactor * 16 * 9.81 / 1024;
	}else{	// RANGE_16G
		return (double)scaleFactor * 32 * 9.81 / 1024;
	}
}

void ADXL345::calibrateAccelerometer() {
	calibrate();
}

int round(double value) {
	return value + (value > 0 ? 0.5 : -0.5);
	// TODO General round
}

void ADXL345::calibrate(uint16 samples) {
	setMeasureEnabled(false);
	setOffset(0, 0, 0);
	FIFOMode lastFIFOMode = getFIFOMode();
	setFIFOMode(FIFO_MODE_BYPASS);
	setMeasureEnabled(true);
	
	int32 avg_x = 0, avg_y = 0, avg_z = 0;
	int16 x, y, z;
	uint32 delay = 1000000 / getDataRateFreq();
	getAcceleration(x, y, z); // Skip first value
	usleep(delay);
	for (int i=0; i<samples; i++) {
		getAcceleration(x, y, z);
		avg_x += x;
		avg_y += y;
		avg_z += z;
		usleep(delay);
	}
	avg_x /= samples;
	avg_y /= samples;
	avg_z /= samples;
	
	Range range = getRange();
	if (RANGE_2G == range || isFullResolutionEnabled()) {
		avg_x = -round(avg_x / 4.0);
		avg_y = -round(avg_y / 4.0);
		avg_z = -round(avg_z / 4.0 - 64 / scaleFactor);
	}else if (RANGE_4G == range){
		avg_x = -round(avg_x / 2.0);
		avg_y = -round(avg_y / 2.0);
		avg_z = -round(avg_z / 2.0 - 64 / scaleFactor);
	}else if (RANGE_8G == range){
		avg_x = -avg_x;
		avg_y = -avg_y;
		avg_z = -(avg_z - 64 / scaleFactor);
	}else{	// RANGE_16G
		avg_x = -avg_x * 2;
		avg_y = -avg_y * 2;
		avg_z = -(avg_z * 2 - 64 / scaleFactor);
	}
	// TODO Implement additional function for LSB per g
	
	setMeasureEnabled(false);
	setOffset(avg_x, avg_y, avg_z);
	setFIFOMode(lastFIFOMode);
	setMeasureEnabled(true);
	getAcceleration(x, y, z); // Skip first value
}

float ADXL345::getScaleFactor() {
	return scaleFactor;
}

void ADXL345::setScaleFactor(float value) {
	if (scaleFactor > 0)
		scaleFactor = value;
}

// DEVID register
uint8 ADXL345::getDeviceID() {
	return bus->readByteData(REG_DEVID);
}

// THRESH_TAP register
uint8 ADXL345::getTapThreshold() {
	return bus->readByteData(REG_THRESH_TAP);
}

void ADXL345::setTapThreshold(uint8 value) {
	bus->writeByteData(REG_THRESH_TAP, value);
}

// OFS* registers
void ADXL345::getOffset(int8& x, int8& y, int8& z) {
	// TODO check if readI2CBlockData is supported
	int8 data[3];
	int8 len = bus->readI2CBlockData(REG_OFSX, 3, (uint8*)data);
	if (3 != len) {
		printf("read operation failed.\n");
		exit(1);
	}
	x = data[0];
	y = data[1];
	z = data[2];
}

void ADXL345::setOffset(int8 x, int8 y, int8 z) {
	// TODO check if writeI2CBlockData is supported
	int8 data[3];
	data[0] = x;
	data[1] = y;
	data[2] = z;
	bus->writeI2CBlockData(REG_OFSX, 3, (uint8*)data);
}

int8 ADXL345::getOffsetX() {
	return bus->readByteData(REG_OFSX);
}

void ADXL345::setOffsetX(int8 value) {
	bus->writeByteData(REG_OFSX, value);
}

int8 ADXL345::getOffsetY() {
	return bus->readByteData(REG_OFSY);
}

void ADXL345::setOffsetY(int8 value) {
	bus->writeByteData(REG_OFSY, value);
}

int8 ADXL345::getOffsetZ() {
	return bus->readByteData(REG_OFSZ);
}

void ADXL345::setOffsetZ(int8 value) {
	bus->writeByteData(REG_OFSZ, value);
}


// DUR register
uint8 ADXL345::getTapDuration() {
	return bus->readByteData(REG_DUR);
}

void ADXL345::setTapDuration(uint8 value) {
	bus->writeByteData(REG_DUR, value);
}


// LATENT register
uint8 ADXL345::getDoubleTapLatency() {
	return bus->readByteData(REG_LATENT);
}

void ADXL345::setDoubleTapLatency(uint8 value) {
	bus->writeByteData(REG_LATENT, value);
}


// WINDOW register
uint8 ADXL345::getDoubleTapWindow() {
	return bus->readByteData(REG_WINDOW);
}

void ADXL345::setDoubleTapWindow(uint8 value) {
	bus->writeByteData(REG_WINDOW, value);
}


// THRESH_ACT register
uint8 ADXL345::getActivityThreshold() {
	return bus->readByteData(REG_THRESH_ACT);
}

void ADXL345::setActivityThreshold(uint8 value) {
	bus->writeByteData(REG_THRESH_ACT, value);
}


// THRESH_INACT register
uint8 ADXL345::getInactivityThreshold() {
	return bus->readByteData(REG_THRESH_INACT);
}

void ADXL345::setInactivityThreshold(uint8 value) {
	bus->writeByteData(REG_THRESH_INACT, value);
}


// TIME_INACT register
uint8 ADXL345::getInactivityTime() {
	return bus->readByteData(REG_TIME_INACT);
}

void ADXL345::setInactivityTime(uint8 value) {
	bus->writeByteData(REG_TIME_INACT, value);
}


// ACT_INACT_CTL register
bool ADXL345::isActivityACEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_ACT_AC_BIT);
}

void ADXL345::setActivityACEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_ACT_AC_BIT, enabled);
}

bool ADXL345::isActivityXEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_ACT_X_BIT);
}

void ADXL345::setActivityXEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_ACT_X_BIT, enabled);
}

bool ADXL345::isActivityYEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_ACT_Y_BIT);
}

void ADXL345::setActivityYEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_ACT_Y_BIT, enabled);
}

bool ADXL345::isActivityZEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_ACT_Z_BIT);
}

void ADXL345::setActivityZEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_ACT_Z_BIT, enabled);
}

bool ADXL345::isInactivityACEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_INACT_AC_BIT);
}

void ADXL345::setInactivityACEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_INACT_AC_BIT, enabled);
}

bool ADXL345::isInactivityXEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_INACT_X_BIT);
}

void ADXL345::setInactivityXEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_INACT_X_BIT, enabled);
}

bool ADXL345::isInactivityYEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_INACT_Y_BIT);
}

void ADXL345::setInactivityYEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_INACT_Y_BIT, enabled);
}

bool ADXL345::isInactivityZEnabled() {
	return bus->readBit(REG_ACT_INACT_CTL, AIC_INACT_Z_BIT);
}

void ADXL345::setInactivityZEnabled(bool enabled) {
	bus->writeBit(REG_ACT_INACT_CTL, AIC_INACT_Z_BIT, enabled);
}


// THRESH_FF register
uint8 ADXL345::getFreefallThreshold() {
	return bus->readByteData(REG_THRESH_FF);
}

void ADXL345::setFreefallThreshold(uint8 value) {
	bus->writeByteData(REG_THRESH_FF, value);
}


// TIME_FF register
uint8 ADXL345::getFreefallTime() {
	return bus->readByteData(REG_TIME_FF);
}

void ADXL345::setFreefallTime(uint8 value) {
	bus->writeByteData(REG_TIME_FF, value);
}


// TAP_AXES register
bool ADXL345::isTapAxisSuppressEnabled() {
	return bus->readBit(REG_TAP_AXES, TAPAXIS_SUP_BIT);
}

void ADXL345::setTapAxisSuppressEnabled(bool enabled) {
	bus->writeBit(REG_TAP_AXES, TAPAXIS_SUP_BIT, enabled);
}

bool ADXL345::isTapAxisXEnabled() {
	return bus->readBit(REG_TAP_AXES, TAPAXIS_X_BIT);
}

void ADXL345::setTapAxisXEnabled(bool enabled) {
	bus->writeBit(REG_TAP_AXES, TAPAXIS_X_BIT, enabled);
}

bool ADXL345::isTapAxisYEnabled() {
	return bus->readBit(REG_TAP_AXES, TAPAXIS_Y_BIT);
}

void ADXL345::setTapAxisYEnabled(bool enabled) {
	bus->writeBit(REG_TAP_AXES, TAPAXIS_Y_BIT, enabled);
}

bool ADXL345::isTapAxisZEnabled() {
	return bus->readBit(REG_TAP_AXES, TAPAXIS_Z_BIT);
}

void ADXL345::setTapAxisZEnabled(bool enabled) {
	bus->writeBit(REG_TAP_AXES, TAPAXIS_Z_BIT, enabled);
}


// ACT_TAP_STATUS register
bool ADXL345::isActivitySourceX() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_ACTX_BIT);
}

bool ADXL345::isActivitySourceY() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_ACTY_BIT);
}

bool ADXL345::isActivitySourceZ() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_ACTZ_BIT);
}

bool ADXL345::isAsleep() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_ASLEEP_BIT);
}

bool ADXL345::isTapSourceX() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_TAPX_BIT);
}

bool ADXL345::isTapSourceY() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_TAPY_BIT);
}

bool ADXL345::isTapSourceZ() {
	return bus->readBit(REG_ACT_TAP_STATUS, TAPSTAT_TAPZ_BIT);
}


// BW_RATE register
bool ADXL345::isLowPowerEnabled() {
	return bus->readBit(REG_BW_RATE, BW_LOWPOWER_BIT);
}

void ADXL345::setLowPowerEnabled(bool enabled) {
	bus->writeBit(REG_BW_RATE, BW_LOWPOWER_BIT, enabled);
}

ADXL345::DataRate ADXL345::getDataRate() {
	return (ADXL345::DataRate)bus->readBits(REG_BW_RATE, BW_RATE_BIT, BW_RATE_LENGTH);
}

float ADXL345::getDataRateFreq() {
	DataRate rate = getDataRate();
	switch (rate) {
		case DATARATE_0_10_HZ:	return 0.10;
		case DATARATE_0_20_HZ:	return 0.20;
		case DATARATE_0_39_HZ:	return 0.39;
		case DATARATE_0_78_HZ:	return 0.78;
		case DATARATE_1_56_HZ:	return 1.56;
		case DATARATE_3_13_HZ:	return 3.13;
		case DATARATE_6_25_HZ:	return 6.25;
		case DATARATE_12_5_HZ:	return 12.5;
		case DATARATE_25_HZ:	return 25;
		case DATARATE_50_HZ:	return 50;
		case DATARATE_100_HZ:	return 100;
		case DATARATE_200_HZ:	return 200;
		case DATARATE_400_HZ:	return 400;
		case DATARATE_800_HZ:	return 800;
		case DATARATE_1600_HZ:	return 1600;
		case DATARATE_3200_HZ:	return 3200;
		default: return 100;
	}
}

void ADXL345::setDataRate(ADXL345::DataRate value) {
	bus->writeBits(REG_BW_RATE, BW_RATE_BIT, BW_RATE_LENGTH, (uint8)value);
}


// POWER_CTL register
bool ADXL345::isLinkEnabled() {
	return bus->readBit(REG_POWER_CTL, PCTL_LINK_BIT);
}

void ADXL345::setLinkEnabled(bool enabled) {
	bus->writeBit(REG_POWER_CTL, PCTL_LINK_BIT, enabled);
}

bool ADXL345::isAutoSleepEnabled() {
	return bus->readBit(REG_POWER_CTL, PCTL_AUTOSLEEP_BIT);
}

void ADXL345::setAutoSleepEnabled(bool enabled) {
	bus->writeBit(REG_POWER_CTL, PCTL_AUTOSLEEP_BIT, enabled);
}

bool ADXL345::isMeasureEnabled() {
	return bus->readBit(REG_POWER_CTL, PCTL_MEASURE_BIT);
}

void ADXL345::setMeasureEnabled(bool enabled) {
	bus->writeBit(REG_POWER_CTL, PCTL_MEASURE_BIT, enabled);
}

bool ADXL345::isSleepEnabled() {
	return bus->readBit(REG_POWER_CTL, PCTL_SLEEP_BIT);
}

void ADXL345::setSleepEnabled(bool enabled) {
	bus->writeBit(REG_POWER_CTL, PCTL_SLEEP_BIT, enabled);
}

ADXL345::Wakeup ADXL345::getWakeupFrequency() {
	return (ADXL345::Wakeup)bus->readBits(REG_POWER_CTL, PCTL_WAKEUP_BIT, PCTL_WAKEUP_LENGTH);
}

void ADXL345::setWakeupFrequency(ADXL345::Wakeup value) {
	bus->writeBits(REG_POWER_CTL, PCTL_WAKEUP_BIT, PCTL_WAKEUP_LENGTH, (uint8)value);
}
	
// INT_ENABLE register
bool ADXL345::isIntDataReadyEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_DATA_READY_BIT);
}

void ADXL345::setIntDataReadyEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_DATA_READY_BIT, enabled);
}

bool ADXL345::isIntSingleTapEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_SINGLE_TAP_BIT);
}

void ADXL345::setIntSingleTapEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_SINGLE_TAP_BIT, enabled);
}

bool ADXL345::isIntDoubleTapEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_DOUBLE_TAP_BIT);
}

void ADXL345::setIntDoubleTapEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_DOUBLE_TAP_BIT, enabled);
}

bool ADXL345::isIntActivityEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_ACTIVITY_BIT);
}

void ADXL345::setIntActivityEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_ACTIVITY_BIT, enabled);
}

bool ADXL345::isIntInactivityEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_INACTIVITY_BIT);
}

void ADXL345::setIntInactivityEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_INACTIVITY_BIT, enabled);
}

bool ADXL345::isIntFreefallEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_FREE_FALL_BIT);
}

void ADXL345::setIntFreefallEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_FREE_FALL_BIT, enabled);
}

bool ADXL345::isIntWatermarkEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_WATERMARK_BIT);
}

void ADXL345::setIntWatermarkEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_WATERMARK_BIT, enabled);
}

bool ADXL345::isIntOverrunEnabled() {
	return bus->readBit(REG_INT_ENABLE, INT_OVERRUN_BIT);
}

void ADXL345::setIntOverrunEnabled(bool enabled) {
	bus->writeBit(REG_INT_ENABLE, INT_OVERRUN_BIT, enabled);
}


// INT_MAP register
ADXL345::IntPin ADXL345::getIntDataReadyPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_DATA_READY_BIT);
}

void ADXL345::setIntDataReadyPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_DATA_READY_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntSingleTapPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_SINGLE_TAP_BIT);
}

void ADXL345::setIntSingleTapPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_SINGLE_TAP_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntDoubleTapPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_DOUBLE_TAP_BIT);
}

void ADXL345::setIntDoubleTapPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_DOUBLE_TAP_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntActivityPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_ACTIVITY_BIT);
}

void ADXL345::setIntActivityPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_ACTIVITY_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntInactivityPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_INACTIVITY_BIT);
}

void ADXL345::setIntInactivityPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_INACTIVITY_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntFreefallPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_FREE_FALL_BIT);
}

void ADXL345::setIntFreefallPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_FREE_FALL_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntWatermarkPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_WATERMARK_BIT);
}

void ADXL345::setIntWatermarkPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_WATERMARK_BIT, (bool)pin);
}

ADXL345::IntPin ADXL345::getIntOverrunPin() {
	return (ADXL345::IntPin)bus->readBit(REG_INT_MAP, INT_OVERRUN_BIT);
}

void ADXL345::setIntOverrunPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_INT_MAP, INT_OVERRUN_BIT, (bool)pin);
}


// INT_SOURCE register
bool ADXL345::isIntDataReady() {
	return bus->readBit(REG_INT_SOURCE, INT_DATA_READY_BIT);
}

bool ADXL345::isIntSingleTap() {
	return bus->readBit(REG_INT_SOURCE, INT_SINGLE_TAP_BIT);
}

bool ADXL345::isIntDoubleTap() {
	return bus->readBit(REG_INT_SOURCE, INT_DOUBLE_TAP_BIT);
}

bool ADXL345::isIntActivity() {
	return bus->readBit(REG_INT_SOURCE, INT_ACTIVITY_BIT);
}

bool ADXL345::isIntInactivity() {
	return bus->readBit(REG_INT_SOURCE, INT_INACTIVITY_BIT);
}

bool ADXL345::isIntFreefall() {
	return bus->readBit(REG_INT_SOURCE, INT_FREE_FALL_BIT);
}

bool ADXL345::isIntWatermark() {
	return bus->readBit(REG_INT_SOURCE, INT_WATERMARK_BIT);
}

bool ADXL345::isIntOverrun() {
	return bus->readBit(REG_INT_SOURCE, INT_OVERRUN_BIT);
}


// DATA_FORMAT register
bool ADXL345::isSelfTestEnabled() {
	return bus->readBit(REG_DATA_FORMAT, FORMAT_SELFTEST_BIT);
}

void ADXL345::setSelfTestEnabled(bool enabled) {
	bus->writeBit(REG_DATA_FORMAT, FORMAT_SELFTEST_BIT, enabled);
}

ADXL345::SPIMode ADXL345::getSPIMode() {
	return (ADXL345::SPIMode)bus->readBit(REG_DATA_FORMAT, FORMAT_SPIMODE_BIT);
}

void ADXL345::setSPIMode(ADXL345::SPIMode mode) {
	bus->writeBit(REG_DATA_FORMAT, FORMAT_SPIMODE_BIT, (bool)mode);
}

ADXL345::IntMode ADXL345::getInterruptMode() {
	return (ADXL345::IntMode)bus->readBit(REG_DATA_FORMAT, FORMAT_INTMODE_BIT);
}

void ADXL345::setInterruptMode(ADXL345::IntMode mode) {
	bus->writeBit(REG_DATA_FORMAT, FORMAT_INTMODE_BIT, (bool)mode);
}

bool ADXL345::isFullResolutionEnabled() {
	return bus->readBit(REG_DATA_FORMAT, FORMAT_FULL_RES_BIT);
}

void ADXL345::setFullResolutionEnabled(bool enabled) {
	bus->writeBit(REG_DATA_FORMAT, FORMAT_FULL_RES_BIT, enabled);
	precision = 0;
}

ADXL345::Justify ADXL345::getDataJustification() {
	return (ADXL345::Justify)bus->readBit(REG_DATA_FORMAT, FORMAT_JUSTIFY_BIT);
}

void ADXL345::setDataJustification(ADXL345::Justify value) {
	bus->writeBit(REG_DATA_FORMAT, FORMAT_JUSTIFY_BIT, (bool)value);
}

ADXL345::Range ADXL345::getRange() {
	return (ADXL345::Range)bus->readBits(REG_DATA_FORMAT, FORMAT_RANGE_BIT, FORMAT_RANGE_LENGTH);
}

void ADXL345::setRange(ADXL345::Range value) {
	bus->writeBits(REG_DATA_FORMAT, FORMAT_RANGE_BIT, FORMAT_RANGE_LENGTH, (uint8)value);
	precision = 0;
}


// DATA* registers
void ADXL345::getAcceleration(int16& x, int16& y, int16& z) {
	// TODO check if readI2CBlockData is supported
	int16 data[3];
	int8 len = bus->readI2CBlockData(REG_DATAX0, 6, (uint8*)data);
	if (6 != len) {
		printf("read operation failed.\n");
		exit(1);
	}
	x = data[0];
	y = data[1];
	z = data[2];
}

int16 ADXL345::getAccelerationX() {
	return bus->readWordData(REG_DATAX0);
}

int16 ADXL345::getAccelerationY() {
	return bus->readWordData(REG_DATAY0);
}

int16 ADXL345::getAccelerationZ() {
	return bus->readWordData(REG_DATAZ0);
}


// FIFO_CTL register
ADXL345::FIFOMode ADXL345::getFIFOMode() {
	return (ADXL345::FIFOMode)bus->readBits(REG_FIFO_CTL, FIFO_MODE_BIT, FIFO_MODE_LENGTH);
}

void ADXL345::setFIFOMode(ADXL345::FIFOMode value) {
	bus->writeBits(REG_FIFO_CTL, FIFO_MODE_BIT, FIFO_MODE_LENGTH, (uint8)value);
}

ADXL345::IntPin ADXL345::getFIFOTriggerInterruptPin() {
	return (ADXL345::IntPin)bus->readBit(REG_FIFO_CTL, FIFO_TRIGGER_BIT);
}

void ADXL345::setFIFOTriggerInterruptPin(ADXL345::IntPin pin) {
	bus->writeBit(REG_FIFO_CTL, FIFO_TRIGGER_BIT, (bool)pin);
}

uint8 ADXL345::getFIFOSamples() {
	return bus->readBits(REG_FIFO_CTL, FIFO_SAMPLES_BIT, FIFO_SAMPLES_LENGTH);
}

void ADXL345::setFIFOSamples(uint8 value) {
	bus->writeBits(REG_FIFO_CTL, FIFO_SAMPLES_BIT, FIFO_SAMPLES_LENGTH, value);
}


// FIFO_STATUS register
bool ADXL345::isFIFOTriggerOccurred() {
	return bus->readBit(REG_FIFO_STATUS, FIFOSTAT_TRIGGER_BIT);
}

uint8 ADXL345::getFIFOSize() {
	return bus->readBits(REG_FIFO_STATUS, FIFOSTAT_SIZE_BIT, FIFOSTAT_SIZE_LENGTH);
}
