#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "DriverIO.h"


int main() {

	DriverIO io;

	io = DriverIO("/sys/class/pwm/pwm6/period_ns");

	printf("%lu %llu\n", io.readUInt32(), io.readUInt64());
	
	io.write((uint32)123456000);

	printf("%lu %llu\n", io.readUInt32(), io.readUInt64());
	
	io.write((uint64)923456000);

	printf("%lu %llu\n", io.readUInt32(), io.readUInt64());
}