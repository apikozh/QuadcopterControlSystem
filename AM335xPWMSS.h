#ifndef AM335XPWMSS_H_
#define AM335XPWMSS_H_

#include "PWM.h"
#include "DriverIO.h"

// AM335x PWMSS (Pulse-Width Modulation Subsystem)

namespace AM335x {

	class EPWM : public PWM {
		
	private:
		//static const char classPath[] = "/sys/class/pwm/";
		//static const char devDir[] = "pwm%d";

		uint32 chipNumber;
		char devDir[40];
		DriverIO *polarityFile, *periodNsFile, *dutyNsFile, *runFile, *ueventFile;	
		
	public:
		EPWM(uint32 chipNumber);
		~EPWM();

		// Device
		virtual void setEnabled(bool value);
		virtual bool isEnabled();
		
		// PWM
		virtual void setPolarity(Polarity value);
		virtual Polarity getPolarity();
		
		virtual void setPeriodNs(uint32 value);
		virtual uint32 getPeriodNs();

		virtual void setDutyNs(uint32 value);
		virtual uint32 getDutyNs();
		
	};

} // AM335x namespace

#endif