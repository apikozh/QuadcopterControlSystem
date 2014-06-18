#include "L3G4200D.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

L3G4200D::L3G4200D(I2CBus* _bus, uint16 address) : bus(_bus->createConnection()) {
	this->address = address;
	bus->setSlaveAddress(address);
	if (getDeviceID() != DEVICE_ID) {
		printf("Device ID is not match for L3G4200D.\n");
		exit(1);
	}
	precision = 0;
	offsetX = offsetY = offsetZ = 0;
}

L3G4200D::~L3G4200D() {
	delete bus;
}

void L3G4200D::setEnabled(bool value) {
	setPowerOnEnabled(value);
}

bool L3G4200D::isEnabled() {
	return isPowerOnEnabled();
}

void L3G4200D::getAngularVelocity(double& rot_x, double& rot_y, double& rot_z) {
	int16 x, y, z;
	getAngularVelocity(x, y, z);
	if (!precision)
		precision = getAngularVelocityPrecision();
	rot_x = x * precision + offsetX;
	rot_y = y * precision + offsetY;
	rot_z = z * precision + offsetZ;
}

double L3G4200D::getAngularVelocityRange() {
	Range range = getRange();
	switch (range) {
		case RANGE_250_DPS: return 250;
		case RANGE_500_DPS: return 500;
		default: return 2000;
	}
	// TODO Check if range values correct
}

double L3G4200D::getAngularVelocityPrecision() {
	Range range = getRange();
	switch (range) {
		case RANGE_250_DPS: return 8.75 / 1000;
		case RANGE_500_DPS: return 17.50 / 1000;
		default: return 70.0 / 1000;
	}
	// TODO Check if precision values correct
}

void L3G4200D::calibrateGyroscope() {
	const uint32 samples = 2500;
	offsetX = offsetY = offsetZ = 0;
	double rot_x, rot_y, rot_z;
	double avg_x = 0, avg_y = 0, avg_z = 0;
	uint32 delay = 1000000 / getDataRateFreq();
	getAngularVelocity(rot_x, rot_y, rot_z);
	usleep(delay);
	for (uint32 i=0; i<samples; i++) {
		getAngularVelocity(rot_x, rot_y, rot_z);
		avg_x += rot_x;
		avg_y += rot_y;
		avg_z += rot_z;
		usleep(delay);
	}
	avg_x /= samples;
	avg_y /= samples;
	avg_z /= samples;
	offsetX = -avg_x;
	offsetY = -avg_y;
	offsetZ = -avg_z;
}


// DEVID register
uint8 L3G4200D::getDeviceID() {
	return bus->readByteData(REG_DEVID);
}


// CTRL_REG1 register
void L3G4200D::setOutputDataRate(L3G4200D::DataRate value) {
	bus->writeBits(REG_CTRL1, CTRL_DATARATE_BIT, CTRL_DATARATE_LENGTH, value);	
}

L3G4200D::DataRate L3G4200D::getOutputDataRate() {
	return (L3G4200D::DataRate)bus->readBits(REG_CTRL1, CTRL_DATARATE_BIT, CTRL_DATARATE_LENGTH);
}

float L3G4200D::getDataRateFreq() {
	DataRate rate = getOutputDataRate();
	switch (rate) {
		case DATARATE_100_HZ:	return 100;
		case DATARATE_200_HZ:	return 200;
		case DATARATE_400_HZ:	return 400;
		case DATARATE_800_HZ:	return 800;
		default: return 100;
	}
}

//	void setBandwidthCutOffMode(uint8_t mode);
//	uint8_t getBandwidthCutOffMode();
//	float getBandwidthCutOff();
void L3G4200D::setPowerOnEnabled(bool value) {
	bus->writeBit(REG_CTRL1, CTRL_POWER_BIT, value);	
}

bool L3G4200D::isPowerOnEnabled() {
	return bus->readBit(REG_CTRL1, CTRL_POWER_BIT);	
}

void L3G4200D::setZEnabled(bool value) {
	bus->writeBit(REG_CTRL1, CTRL_ZENABLED_BIT, value);	
}

bool L3G4200D::isZEnabled() {
	return bus->readBit(REG_CTRL1, CTRL_ZENABLED_BIT);	
}

void L3G4200D::setYEnabled(bool value) {
	bus->writeBit(REG_CTRL1, CTRL_YENABLED_BIT, value);	
}

bool L3G4200D::isYEnabled() {
	return bus->readBit(REG_CTRL1, CTRL_YENABLED_BIT);	
}

void L3G4200D::setXEnabled(bool value) {
	bus->writeBit(REG_CTRL1, CTRL_XENABLED_BIT, value);	
}

bool L3G4200D::isXEnabled() {
	return bus->readBit(REG_CTRL1, CTRL_XENABLED_BIT);	
}


// CTRL_REG4 register
void L3G4200D::setBlockDataUpdateEnabled(bool value) {
	bus->writeBit(REG_CTRL4, CTRL_BDU_BIT, value);
}

bool L3G4200D::getBlockDataUpdateEnabled() {
	return bus->readBit(REG_CTRL4, CTRL_BDU_BIT);
}

L3G4200D::Range L3G4200D::getRange() {
	return (L3G4200D::Range)bus->readBits(REG_CTRL4, CTRL_RANGE_BIT, CTRL_RANGE_LENGTH);
}

void L3G4200D::setRange(L3G4200D::Range value) {
	bus->writeBits(REG_CTRL4, CTRL_RANGE_BIT, CTRL_RANGE_LENGTH, (uint8)value);
	precision = 0;
}


// OUT_* registers
void L3G4200D::getAngularVelocity(int16& x, int16& y, int16& z) {
	// TODO check if readI2CBlockData is supported
	int16 data[3];
	int8 len = bus->readI2CBlockData(REG_OUT_X_L | ADDRESS_AUTOINCREMENT, 6, (uint8*)data);
	if (6 != len) {
		printf("read operation failed.\n");
		exit(1);
	}
	x = data[0];
	y = data[1];
	z = data[2];
}

int16 L3G4200D::getAngularVelocityX() {
	return bus->readWordData(REG_OUT_X_L | ADDRESS_AUTOINCREMENT);
}

int16 L3G4200D::getAngularVelocityY() {
	return bus->readWordData(REG_OUT_Y_L | ADDRESS_AUTOINCREMENT);
}

int16 L3G4200D::getAngularVelocityZ() {
	return bus->readWordData(REG_OUT_Z_L | ADDRESS_AUTOINCREMENT);
}

