#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "LocalI2CBus.h"
#include "ADXL345.h"
#include "L3G4200D.h"
#include "AM335xPWMSS.h"
#include "Motor.h"
#include "Controller.h"

Controller ctrl;

void interruptHandler(int signal){
	printf("Caught signal %d\n", signal);
	ctrl.terminate();
}

int main(int argc, char *argv[]) {

	printf("A.Pikozh Quadcopter controller v1.0\n\n");
	
	// Set interrupt handler
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = interruptHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	//FILE* log = fopen("log.txt", "w");
	
	// Read params
	double pidP = 0, pidI = 0, pidD = 0;//, throttle; 
	if (argc == 4) {
		sscanf(argv[1], "%lf", &pidP);
		sscanf(argv[2], "%lf", &pidI);
		sscanf(argv[3], "%lf", &pidD);
		//sscanf(argv[4], "%lf", &throttle);
		//throttle = constrain(throttle, 15.0, 40.0);
		printf("PID: %f %f %f\n", pidP, pidI, pidD);
		//fprintf(log, "PID: %f %f %f\n", pidP, pidI, pidD);
		//printf("Throttle: %f\n", throttle);
		//fprintf(log, "Throttle: %f\n", throttle);
	}
	
	// Create UDP socket server
	printf("Initializing UDP socket server... ");fflush(stdout);
	UDPSocket conn;
	conn.bind(6000);
	conn.setNonBlockingEnabled(true);
	printf("OK\n");
	
	printf("Initializing I2C bus... ");fflush(stdout);
	LocalI2CBus bus(1);
	printf("OK\n");
	
	// Accelerometer
	printf("Initializing accelerometer (ADXL345)... ");fflush(stdout);
	ADXL345 acc(&bus);		
	acc.setEnabled(false);
	acc.setDataRate(ADXL345::DATARATE_800_HZ); // Prev test with 100
	acc.setRange(ADXL345::RANGE_16G);
	acc.setFullResolutionEnabled(true);
	acc.setOffset(2, 0, 16);
	acc.setEnabled(true);
	printf("OK\n");
	
	// Calibrate Accelerometer
	//acc.calibrateAccelerometer(); // calibrate(100);

	// Gyroscope
	printf("Initializing gyroscope (L3G4200D)... ");fflush(stdout);
	L3G4200D gyro(&bus);
	gyro.setEnabled(false);
	gyro.setOutputDataRate(L3G4200D::DATARATE_800_HZ);
	gyro.setRange(L3G4200D::RANGE_500_DPS);
	//gyro.setBlockDataUpdateEnabled(false);
	gyro.setEnabled(true);
	printf("OK\n");

	// Calibrate Gyroscope
	printf("Calibrating gyroscope... ");fflush(stdout);
	gyro.calibrateGyroscope();
	printf("DONE\n");
	
	/*timespec ts, ts2;

	double rota[3], rot[3];
	for (int i=0; i<5000; i++) {
		gyro.getAngularVelocity(rot[0], rot[1], rot[2]);
		rota[0] += rot[0];
		rota[1] += rot[1];
		rota[2] += rot[2];
		clock_gettime(CLOCK_REALTIME, &ts2);
		//usleep(10000);
	}
	rota[0] /= 10000;
	rota[1] /= 10000;
	rota[2] /= 10000;*/
		

	// Print calibration values
	/*int8 ox, oy, oz;
	acc.getOffset(ox, oy, oz);
	printf("GYRO:\t%.5f\t%.5f\t%.5f\tACC:\t%d\t%d\t%d\n", rota[0], rota[1], rota[2], ox, oy, oz);
	fprintf(log, "GYRO:\t%.5f\t%.5f\t%.5f\tACC:\t%d\t%d\t%d\n", rota[0], rota[1], rota[2], ox, oy, oz);
	usleep(3000000);

	if (argc != 5 || terminated) {
		fclose(log);
		return 0;
	}*/

	// PWM & Motors
	printf("Initializing PWM channels and motors... ");fflush(stdout);
	AM335x::EPWM pwm1(3), pwm2(6), pwm3(4), pwm4(5);
	Motor motorFL(&pwm1, PWM::POLARITY_HIGH, 2500000);
	Motor motorFR(&pwm2, PWM::POLARITY_HIGH, 2500000);
	Motor motorBL(&pwm3, PWM::POLARITY_HIGH, 2500000);
	Motor motorBR(&pwm4, PWM::POLARITY_HIGH, 2500000);
	motorFL.enable();
	motorFR.enable();
	motorBL.enable();
	motorBR.enable();
	printf("OK\n");
		
	ctrl.setSensors(&gyro, &acc);
	ctrl.setMotors(&motorFL, &motorFR, &motorBL, &motorBR);
	ctrl.setPID(PID_LEVEL, pidP, pidI, pidD);
	ctrl.setConnection(&conn);
	
	printf("Let's start!\n");
	ctrl.run();
	
	printf("Goodbye!\n");
	sleep(2);
}