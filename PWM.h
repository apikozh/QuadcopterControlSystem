#ifndef PWM_H_
#define PWM_H_

#include "Device.h"

// Pulse Width Modulator 

/*abstract*/ struct PWM : virtual Device {
	
	enum Polarity {
		POLARITY_HIGH = 0,	// active high
		POLARITY_LOW = 1	// active low (driver default)
	};
	
	virtual void setPolarity(Polarity value) = 0;
	virtual Polarity getPolarity() = 0;
	
	virtual void setPeriodNs(uint32 value) = 0;
	virtual void setPeriodHz(float value) {
		setPeriodNs(1000000000.0 / value);
	}

	virtual uint32 getPeriodNs() = 0;
	virtual float getPeriodHz() {
		return 1000000000.0 / getPeriodNs();
	}

	virtual void setDutyNs(uint32 value) = 0;
	virtual void setDutyHz(float value) {
		setDutyNs(1000000000.0 / value);
	}
	virtual void setDutyPercent(float value) {
		setDutyNs(value * getPeriodNs() / 100.0);
	}
	
	virtual uint32 getDutyNs() = 0;
	virtual float getDutyHz() {
		return 1000000000.0 / getDutyNs();
	}
	virtual float getDutyPercent() {
		return getDutyNs() * 100.0 / getPeriodNs();
	}
		
	// TODO getRange
	
};

class PWMChannel;

/*abstract*/ class PWMSystem {

	virtual PWMChannel* operator [](uint32 index) = 0;
	virtual uint32 getChannelsNumber() = 0;
	
	virtual bool isChannelsSharePolarity() = 0;
	virtual bool isChannelsSharePeriod() = 0;

	// TODO period, polarity, getRange, setScaleFactor, ...
	
};

/*abstract*/ class PWMChannel : public virtual PWM {
	
private:
	PWMSystem* baseSystem;
	
public:
	PWMChannel(PWMSystem* baseSystem) {
		this->baseSystem = baseSystem;
	}
	
	virtual PWMSystem* getBaseSystem() {
		return baseSystem;
	}
	
	// TODO phase/offset from other channels
};


#endif