#ifndef DEVICE_H_
#define DEVICE_H_

#include "types.h"

/*abstract*/ struct Device {
	
	virtual void setEnabled(bool value) = 0;
	virtual bool isEnabled() = 0;

	virtual void enable() {
		setEnabled(true);
	}

	virtual void disable() {
		setEnabled(false);
	}

	//virtual char* getDeviceName() = 0;
	
};

#endif