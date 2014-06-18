#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_

// Temperature sensor

/*abstract*/ struct TempSensor : virtual Device {
	
	/*
		Return value unit - degrees celcius
	*/	
	virtual double getTemperature() = 0;

};

#endif