#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "AM335xPWMSS.h"

void stat(AM335x::EPWM& pwm) {
	printf("isEnabled: %s\n", pwm.isEnabled() ? "true" : "false");
	printf("getPolarity: %lu\n", (uint32)pwm.getPolarity());
	printf("getPeriodNs: %lu\n", pwm.getPeriodNs());
	printf("getDutyNs: %lu\n", pwm.getDutyNs());
}

int main() {

	AM335x::EPWM pwm(5);
	sleep(10);

	stat(pwm);	
	pwm.setPolarity(PWM::POLARITY_HIGH);
	stat(pwm);
	pwm.setPeriodNs(923456000);
	stat(pwm);
	pwm.setDutyNs(123456000);
	stat(pwm);
	pwm.enable();
	stat(pwm);
	
}