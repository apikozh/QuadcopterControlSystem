#include "Motor.h"

#include <unistd.h>

Motor::Motor(PWM* pwm, PWM::Polarity polarity, uint32 periodNs, 
			uint32 stopDutyNs, uint32 minDutyNs, uint32 maxDutyNs)
{
	this->pwm = pwm;
	pwm->setPolarity(polarity);
	pwm->setPeriodNs(periodNs);
	pwm->setDutyNs(stopDutyNs);
	
	setStopDuty(stopDutyNs);
	setMinMaxDuty(minDutyNs, maxDutyNs);
}

Motor::~Motor() {
	pwm->setDutyNs(stopDutyNs);
	//usleep(10000);
	//pwm->disable();
}

// Device
void Motor::setEnabled(bool value) {
	pwm->setEnabled(value);
}

bool Motor::isEnabled() {
	return pwm->isEnabled();
}

void Motor::setMinMaxDuty(uint32 minDutyNs, uint32 maxDutyNs) {
	this->minDutyNs = minDutyNs;
	this->maxDutyNs = maxDutyNs;
}

void Motor::setStopDuty(uint32 stopDutyNs) {
	this->stopDutyNs = stopDutyNs;
}

void Motor::setPercent(double value) {
	uint32 dutyNs = minDutyNs + value * (maxDutyNs - minDutyNs) / 100.0;
	if (dutyNs < minDutyNs)
		dutyNs = minDutyNs;
	if (dutyNs > maxDutyNs)
		dutyNs = maxDutyNs;
	pwm->setDutyNs(dutyNs); 
}

double Motor::getPercent() {
	return (pwm->getDutyNs() - minDutyNs) * 100.0 / (maxDutyNs - minDutyNs);
}

void Motor::start() {
	if (pwm->getDutyNs() < minDutyNs) {
		pwm->setDutyNs(minDutyNs);
	}
}

void Motor::stop() {
	pwm->setDutyNs(stopDutyNs);
}

bool Motor::isRunning() {
	return pwm->getDutyNs() > stopDutyNs;
}
