#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "LocalI2CBus.h"
#include "ADXL345.h"


int main() {

	LocalI2CBus bus(1);
	
	ADXL345 acc(&bus);
		
	acc.setRange(ADXL345::RANGE_16G);
	acc.setFullResolutionEnabled(true);
	acc.setEnabled(true);

	int16 x, y, z;
	double acc_x, acc_y, acc_z;
	
	while (true) {
		acc.getLinearAcceleration(acc_x, acc_y, acc_z);
		acc.getAcceleration(x, y, z);
		printf("\t%.2lf\t%.2lf\t%.2lf\t%d\t%d\t%d\n", acc_x, acc_y, acc_z, x, y, z);
		usleep(100000);
	}

}