#ifndef BAROSENSOR_H_
#define BAROSENSOR_H_

// Barometer

/*abstract*/ struct BaroSensor : virtual Device {
	/*
		Return value unit - Pa (pascal)
	*/	
	virtual double getPressure() = 0;
	
	/*
		Param seaLevelPressure - ground pressure
		Return value unit - m (meter)
	*/
	virtual double getAltitude(double seaLevelPressure) = 0;

};

#endif