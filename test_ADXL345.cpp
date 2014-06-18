#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "LocalI2CBus.h"
#include "ADXL345.h"

void stats(ADXL345& a) {
	int8 ox, oy, oz;
	a.getOffset(ox, oy, oz);
	printf("getOffset: %d %d %d\n", ox, oy, oz);
	printf("getOffsetZYZ: %d %d %d\n", a.getOffsetX(), a.getOffsetY(), a.getOffsetZ());
	printf("getFreefallThreshold: %d\n", a.getFreefallThreshold());
	printf("getFreefallTime: %d\n", a.getFreefallTime());

	printf("isAutoSleepEnabled: %d\n", a.isAutoSleepEnabled());
	printf("isMeasureEnabled: %d\n", a.isMeasureEnabled());
	printf("isSleepEnabled: %d\n", a.isSleepEnabled());
	
	printf("isSelfTestEnabled: %d\n", a.isSelfTestEnabled());
	printf("isFullResolutionEnabled: %d\n", a.isFullResolutionEnabled());
	printf("getDataJustification: %d\n", a.getDataJustification());
	printf("getRange: %d\n", a.getRange());

	printf("isEnabled: %d\n", a.isEnabled());

	int16 x, y, z;
	a.getAcceleration(x, y, z);
	printf("getAcceleration: %d %d %d\n", x, y, z);
	printf("getAccelerationZYZ: %d %d %d\n", a.getAccelerationX(), a.getAccelerationY(), a.getAccelerationZ());
	
	double acc_x, acc_y, acc_z;
	a.getLinearAcceleration(acc_x, acc_y, acc_z);
	printf("getLinearAcceleration: %lf %lf %lf\n", acc_x, acc_y, acc_z);
	printf("getLinearAccelerationRange: %lf\n", a.getLinearAccelerationRange());
	printf("getLinearAccelerationPrecision: %lf\n", a.getLinearAccelerationPrecision());

	printf("\n");
}

void vals(ADXL345& a) {
	int16 x, y, z;
	a.getAcceleration(x, y, z);
	printf("getAcceleration: %d %d %d\n", x, y, z);

	double acc_x, acc_y, acc_z;
	a.getLinearAcceleration(acc_x, acc_y, acc_z);
	printf("getLinearAcceleration: %lf %lf %lf\n", acc_x, acc_y, acc_z);

	printf("\n");
}

int main() {

	LocalI2CBus bus(1);
	
	ADXL345 acc(&bus);
		
	stats(acc);
	
	printf("setOffset 1, 2, 3\n\n");
	acc.setOffset(1, 2, 3);
	stats(acc);

	printf("setOffset 0, 0, 0\n\n");
	acc.setOffsetX(0);
	acc.setOffsetY(0);
	acc.setOffsetZ(0);
	stats(acc);

	printf("setFFThreshold 10; setFFTime 15\n\n");
	acc.setFreefallThreshold(10);
	acc.setFreefallTime(15);
	stats(acc);

	printf("setSleepEnabled true\n\n");
	acc.setSleepEnabled(true);
	stats(acc);

	printf("setSleepEnabled false\n\n");
	acc.setSleepEnabled(false);
	stats(acc);

	printf("setAutoSleepEnabled true; setMeasureEnabled true\n\n");
	acc.setAutoSleepEnabled(true);
	acc.setMeasureEnabled(true);
	stats(acc);

	printf("setAutoSleepEnabled false; setMeasureEnabled false\n\n");
	acc.setAutoSleepEnabled(false);
	acc.setMeasureEnabled(false);
	stats(acc);

	printf("setRange 16g\n\n");
	acc.setRange(ADXL345::RANGE_16G);
	stats(acc);
	
	printf("setFullResolutionEnabled true\n\n");
	acc.setFullResolutionEnabled(true);
	stats(acc);

	printf("setEnabled true\n\n");
	acc.setEnabled(true);

	for(int i=0; i<10; i++) {
		vals(acc);
		sleep(1);
	}
	
	printf("setSelfTestEnabled true\n\n");
	acc.setSelfTestEnabled(true);

	for(int i=0; i<10; i++) {
		vals(acc);
		sleep(1);
	}
	
	printf("setSelfTestEnabled false\n\n");
	acc.setSelfTestEnabled(false);

	for(int i=0; i<10; i++) {
		vals(acc);
		sleep(1);
	}
	
}