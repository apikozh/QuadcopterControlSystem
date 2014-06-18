#include <stdio.h>
#include <stdlib.h>

#include "LocalI2CBus.h"

int main() {

	LocalI2CBus bus1(1);
	LocalI2CBus bus2(1);
	
	bus1.setSlaveAddress(0x53);
	bus2.setSlaveAddress(0x69);
	
	printf("%x\n", bus1.readByteData(0));
	printf("%x\n", bus2.readByteData(0));

	bus1.setSlaveAddress(0x69);
	bus2.setSlaveAddress(0x53);

	printf("%x\n", bus1.readByteData(0));
	printf("%x\n", bus2.readByteData(0));
}