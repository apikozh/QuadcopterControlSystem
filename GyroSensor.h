#ifndef GYROSENSOR_H_
#define GYROSENSOR_H_

#include "Device.h"

// Gyroscope

/*abstract*/ struct GyroSensor : virtual Device {
	
	/*
		Out params value unit - dps (degrees/sec)
	*/	
	virtual void getAngularVelocity(double& rot_x, double& rot_y, double& rot_z) = 0;

	/*
		[-range ... +range]
	*/	
	virtual double getAngularVelocityRange() = 0;

	virtual double getAngularVelocityPrecision() = 0;

	virtual void calibrateGyroscope() = 0;
	
};

#endif