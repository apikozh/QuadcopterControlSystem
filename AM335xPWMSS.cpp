#include "AM335xPWMSS.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"

using namespace AM335x;

EPWM::EPWM(uint32 chipNumber) {
	this->chipNumber = chipNumber;
	char fileName[40], chipNum[10];
	snprintf(fileName, 39, "/sys/class/pwm/pwm%d", chipNumber);
	strcpy(devDir, fileName);
	int32 strLen;
	if (access(devDir, F_OK) != 0) {
		int32 exportFile = open("/sys/class/pwm/export", O_WRONLY);
		if (exportFile < 0) {
			printf("Failed to open the export file.\n");
			exit(1);
		}
		snprintf(chipNum, 9, "%d\n", chipNumber);
		strLen = strlen(chipNum);
		if (write(exportFile, chipNum, strLen) != strLen) {
			printf("Failed to export PWM chip.\n");
			exit(1);
		}
		close(exportFile);
	}
	
	strLen = strlen(fileName);
	
	strcpy(fileName + strLen, "/polarity");
	polarityFile = new DriverIO(fileName);
	
	strcpy(fileName + strLen, "/period_ns");
	periodNsFile = new DriverIO(fileName);
	
	strcpy(fileName + strLen, "/duty_ns");
	dutyNsFile = new DriverIO(fileName);
	
	strcpy(fileName + strLen, "/run");
	runFile = new DriverIO(fileName);
	
}

EPWM::~EPWM() {
	disable();

	if (access(devDir, F_OK) == 0) {
		int32 unexportFile = open("/sys/class/pwm/unexport", O_WRONLY);
		if (unexportFile < 0) {
			printf("Failed to open the unexport file.\n");
			exit(1);
		}
		char chipNum[10];
		snprintf(chipNum, 9, "%d\n", chipNumber);
		int32 strLen = strlen(chipNum);
		if (write(unexportFile, chipNum, strLen) != strLen) {
			printf("Failed to unexport PWM chip.\n");
			//exit(1);
		}
		close(unexportFile);
	}
	
	delete polarityFile;
	delete periodNsFile;
	delete dutyNsFile;
	delete runFile;
}

// Device
void EPWM::setEnabled(bool value) {
	runFile->write(value ? "1" : "0");
}

bool EPWM::isEnabled() {
	return runFile->readInt32() == 1;
}

// PWM
void EPWM::setPolarity(PWM::Polarity value) {
	polarityFile->write(POLARITY_HIGH == value ? "0" : "1");
}

PWM::Polarity EPWM::getPolarity() {
	return polarityFile->readInt32() == 0 ? POLARITY_HIGH : POLARITY_LOW;
}

void EPWM::setPeriodNs(uint32 value) {
	periodNsFile->write(value);
}

uint32 EPWM::getPeriodNs() {
	return periodNsFile->readUInt32();
}

void EPWM::setDutyNs(uint32 value) {
	if (value <= getPeriodNs())
		dutyNsFile->write(value);
}

uint32 EPWM::getDutyNs() {
	return dutyNsFile->readUInt32();
}
