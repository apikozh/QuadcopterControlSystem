#ifndef ACCELSENSOR_H_
#define ACCELSENSOR_H_

#include "Device.h"

// Accelerometer

/*abstract*/ struct AccelSensor : virtual Device {
	
	/*
		Out params value unit - meters per second squared
	*/	
	virtual void getLinearAcceleration(double& acc_x, double& acc_y, double& acc_z) = 0;

	/*
		[-range ... +range]
	*/	
	virtual double getLinearAccelerationRange() = 0;

	virtual double getLinearAccelerationPrecision() = 0;

	virtual void calibrateAccelerometer() = 0;
	
};

#endif