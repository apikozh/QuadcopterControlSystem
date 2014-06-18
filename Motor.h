#ifndef MOTOR_H_
#define MOTOR_H_

#include "Device.h"
#include "PWM.h"

// Custom motor controller

class Motor : public Device {
	PWM* pwm;
	uint32 stopDutyNs, minDutyNs, maxDutyNs;
public:
	Motor(PWM* pwm, PWM::Polarity polarity = PWM::POLARITY_HIGH, 
		uint32 periodNs = 20000000, uint32 stopDutyNs = 1000000, 
		uint32 minDutyNs = 1150000, uint32 maxDutyNs = 2000000);
	~Motor();
	
	// Device
	virtual void setEnabled(bool value);
	virtual bool isEnabled();

	virtual void setMinMaxDuty(uint32 minDutyNs, uint32 maxDutyNs);
	virtual void setStopDuty(uint32 stopDutyNs);
	
	virtual void setPercent(double value);
	virtual double getPercent();
	
	virtual void start();
	virtual void stop();
	virtual bool isRunning();
	
};



#endif