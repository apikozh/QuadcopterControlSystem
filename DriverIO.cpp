#include "DriverIO.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Class to R/W through sysfs interface

//DriverIO::DriverIO() : file(-1), mode(MODE_NONE) {}

DriverIO::DriverIO(char* fileName, DriverIO::Mode mode) {
	this->mode = mode;
	int32 omode;
	switch (mode) {
		case MODE_READ: omode = O_RDONLY; break;
		case MODE_WRITE: omode = O_WRONLY; break;
		case MODE_READWRITE: omode = O_RDWR; break;
		default:
			printf("Invalid file open mode.\n");
			exit(1);
	}
	file = open(fileName, omode);
	//printf("FN: %s\n", fileName);
	if (file < 0) {
			printf("Failed to open sysfs file.\n");
			exit(1);
	}
}

DriverIO::~DriverIO() {
	close(file);
}

void DriverIO::write(int32 value) {
	char buf[15];
	snprintf(buf, 14, "%li", value);
	write(buf);
}

void DriverIO::write(uint32 value) {
	char buf[15];
	snprintf(buf, 14, "%lu", value);
	write(buf);
}

void DriverIO::write(int64 value) {
	char buf[30];
	snprintf(buf, 29, "%lli", value);
	write(buf);
}

void DriverIO::write(uint64 value) {
	char buf[30];
	snprintf(buf, 29, "%llu", value);
	write(buf);
}

void DriverIO::write(double value) {
	char buf[30];
	snprintf(buf, 29, "%f", value);
	write(buf);
}

void DriverIO::write(const char* value) {
	if (file < 0 || mode & MODE_WRITE == 0)
		return; // TODO Error
	uint32 len = strlen(value);
	if (::write(file, value, len) != len) {
		if (EINVAL == errno) {
			printf("Error writing to file\n");
			exit(1);
		}	
	}

}

int32	DriverIO::readInt32() {
	char buf[30];
	int32 value;
	read(buf, 30);
	sscanf(buf, "%li", &value);
	return value;
}

uint32	DriverIO::readUInt32() {
	char buf[30];
	uint32 value;
	read(buf, 30);
	sscanf(buf, "%lu", &value);
	return value;
}

int64	DriverIO::readInt64() {
	char buf[30];
	int64 value;
	read(buf, 30);
	sscanf(buf, "%lli", &value);
	return value;
}

uint64	DriverIO::readUInt64() {
	char buf[30];
	uint64 value;
	read(buf, 30);
	sscanf(buf, "%llu", &value);
	return value;
}

double	DriverIO::readDouble() {
	char buf[30];
	double value;
	read(buf, 30);
	sscanf(buf, "%f", &value);
	return value;
}

void	DriverIO::read(char* value, int maxLen) {
	if (file < 0 || mode & MODE_READ == 0)
		return; // TODO Error
	lseek(file, 0, SEEK_SET);
	int32 len = ::read(file, value, maxLen - 1);
	if (len < 0) {
		printf("Error reading from file\n");
		exit(1);
	}
	value[len] = 0;
}
