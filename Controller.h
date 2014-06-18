#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <time.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "UDPSocket.h"
#include "msg.h"

double constrain(double val, double min, double max) {
	if (val < min)
		val = min;
	if (val > max)
		val = max;
	return val;
}

void rotateV(double* v, double* delta) {
	double v_tmp[3];
	v_tmp[0] = v[0];
	v_tmp[1] = v[1];
	v_tmp[2] = v[2];

	v[2] += - delta[1]  * v_tmp[0] + delta[0] * v_tmp[1];
	v[0] += delta[1]  * v_tmp[2] - delta[2]   * v_tmp[1];
	v[1] += - delta[0] * v_tmp[2] + delta[2]   * v_tmp[0];
}

class Controller {

private:
	enum MotorPos{
		FRONT_LEFT,
		FRONT_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		MOTORS_NUM
	};
	
	bool terminated;
	GyroSensor* gyro;
	AccelSensor* accel;
	Motor* motors[MOTORS_NUM];
	PID pid[PID_AXIS_NUMBER];
	UDPSocket* conn;
	SocketAddress debugAddress;
	uint8 logLevel;
	uint32 logSendPeriod;

	double throttle;
	double pitch, roll, yaw;
	bool armed;
	
public:
	Controller() : terminated(false), gyro(NULL), accel(NULL), 
		armed(false), throttle(0), pitch(0), roll(0), yaw(0), conn(NULL), logLevel(0), logSendPeriod(20)
	{
		for (int i=0; i<MOTORS_NUM; i++)
			motors[i] = NULL;
		for (int i=0; i<PID_AXIS_NUMBER; i++) {
			pid[i].P = 0;
			pid[i].I = 0;
			pid[i].D = 0;
		}
	}

	~Controller() {}
	
	void setSensors(GyroSensor* gyro, AccelSensor* accel) {
		this->gyro = gyro;
		this->accel = accel;
	}
	
	void setMotors(Motor* frontLeft, Motor* frontRight, Motor* backLeft, Motor* backRight) {
		motors[FRONT_LEFT] = frontLeft;
		motors[FRONT_RIGHT] = frontRight;
		motors[BACK_LEFT] = backLeft;
		motors[BACK_RIGHT] = backRight;
	}
	
	void setPID(PIDAxis axis, float pidP, float pidI, float pidD) {
		this->pid[axis].P = pidP;
		this->pid[axis].I = pidI;
		this->pid[axis].D = pidD;
	}
	
	void setConnection(UDPSocket* conn) {
		this->conn = conn;
	}
	
	void run() {
		if (!gyro || !accel || !motors[FRONT_LEFT] || !motors[FRONT_RIGHT] || 
			!motors[BACK_LEFT] || !motors[BACK_RIGHT] || !conn)
		{
			return;
		}
		
		timespec lastTS, currTS, lastLogTS;
		
		// Calc clock_gettime delay
		clock_gettime(CLOCK_REALTIME, &lastTS);
		for (int i=0; i<1000; i++)
			clock_gettime(CLOCK_REALTIME, &currTS);
		int gettimeDelayNs = ((currTS.tv_sec-lastTS.tv_sec)*1000000000 + currTS.tv_nsec-lastTS.tv_nsec) / 1000;
		
		// Vars
		double acc[3], fAcc[3] = {0, 0, 0}, mag;
		double rot[3], fRot[3] = {0, 0, 0}, eRot[3], iRot[3] = {0, 0, 0}, lRot[3] = {0, 0, 0};
		double ang[3], dAng[3], lAng[3], eAng[3], iAng[3] = {0, 0, 0};
		double interval, gVec[3] = {0, 0, 9.81}, pidVal[3], P[3], I[3], D[3], mFL, mFR, mBL, mBR; 
		
		uint32 counter = 0;
		LogMsg4 log;
		
		accel->getLinearAcceleration(fAcc[0], fAcc[1], fAcc[2]);
		gVec[0] = fAcc[0]; gVec[1] = fAcc[1]; gVec[2] = fAcc[2];
		lAng[0] = atan2(gVec[1], sqrt(gVec[0]*gVec[0] + gVec[2]*gVec[2])) * 180.0/M_PI;
		lAng[1] = atan2(gVec[0], gVec[2]) * 180.0/M_PI;
		lAng[2] = 0;
		clock_gettime(CLOCK_REALTIME, &lastTS);
		lastLogTS = lastTS;
				
		// Main loop
		while (!terminated) {
			accel->getLinearAcceleration(acc[0], acc[1], acc[2]);
			fAcc[0] = (15 * fAcc[0] + acc[0]) / 16;
			fAcc[1] = (15 * fAcc[1] + acc[1]) / 16;
			fAcc[2] = (15 * fAcc[2] + acc[2]) / 16;
			mag = fAcc[0]*fAcc[0]+fAcc[1]*fAcc[1]+fAcc[2]*fAcc[2];

			gyro->getAngularVelocity(rot[0], rot[1], rot[2]);
			//rot[0] *= 3.14 / 180; rot[1] *= 3.14 / 180; rot[2] *= 3.14 / 180;
			fRot[0] = (rot[0] + 15 * fRot[0]) / 16;
			fRot[1] = (rot[1] + 15 * fRot[1]) / 16;
			fRot[2] = (rot[2] + 15 * fRot[2]) / 16;		

			clock_gettime(CLOCK_REALTIME, &currTS);
			interval = ((currTS.tv_sec-lastTS.tv_sec)*1000000000 + currTS.tv_nsec-lastTS.tv_nsec - gettimeDelayNs)/1000000000.0;
			lastTS = currTS;
			
			dAng[0] = -fRot[0] * interval * M_PI/180.0;
			dAng[1] = -fRot[1] * interval * M_PI/180.0;
			dAng[2] = -fRot[2] * interval * M_PI/180.0;
			//ang[0] += dAng[0];
			//ang[1] += dAng[1];
			//ang[2] += dAng[2];
			rotateV(gVec, dAng);
			mag = mag * 100.0 / (9.81 * 9.81);
			if (72.0 < mag && mag < 133.0) {
				gVec[0] = (gVec[0] * 600 + fAcc[0]) / 601;
				gVec[1] = (gVec[1] * 600 + fAcc[1]) / 601;
				gVec[2] = (gVec[2] * 600 + fAcc[2]) / 601;
			}
			ang[0] = atan2(gVec[1], sqrt(gVec[0]*gVec[0] + gVec[2]*gVec[2])) * 180.0/M_PI;
			ang[1] = -atan2(gVec[0], gVec[2]) * 180.0/M_PI;
			
			if (armed) {
				eRot[0] = fRot[0] - pitch;
				eRot[1] = fRot[1] - roll;
				eRot[2] = fRot[2] - yaw;
				
				iRot[0] = constrain(iRot[0] + eRot[0]*interval, -1000000.0, +1000000.0);
				iRot[1] = constrain(iRot[1] + eRot[1]*interval, -1000000.0, +1000000.0);
				iRot[2] = constrain(iRot[2] + eRot[2]*interval, -1000000.0, +1000000.0);

				eAng[0] = ang[0] - pitch;
				eAng[1] = ang[1] - roll;
				eAng[2] = ang[2] - yaw;
				
				iAng[0] = constrain(iAng[0] + eAng[0]*interval, -1000000.0, +1000000.0);
				iAng[1] = constrain(iAng[1] + eAng[1]*interval, -1000000.0, +1000000.0);
				iAng[2] = constrain(iAng[2] + eAng[2]*interval, -1000000.0, +1000000.0);
				
				P[0] = eAng[0] * pid[PID_LEVEL].P;
				I[0] = iAng[0] * pid[PID_LEVEL].I; 
				D[0] = fRot[0] * pid[PID_LEVEL].D;//(ang[0]-lAng[0])/interval * pid[PID_LEVEL].D;
				
				P[1] = eAng[1] * pid[PID_LEVEL].P;
				I[1] = iAng[1] * pid[PID_LEVEL].I;
				D[1] = fRot[1] * pid[PID_LEVEL].D;//(ang[1]-lAng[1])/interval * pid[PID_LEVEL].D;

				P[2] = eRot[2] * pid[PID_YAW].P;
				I[2] = iRot[2] * pid[PID_YAW].I;
				D[2] = 0;//(fRot[2] - lRot[2])/interval * pid[PID_YAW].D;//(ang[1]-lAng[1])/interval * pid[PID_LEVEL].D;
				
				pidVal[0] = P[0] + I[0] + D[0];
				pidVal[1] = P[1] + I[1] + D[1];
				pidVal[2] = P[2] + I[2] + D[2];
								
				mFL = throttle - pidVal[0] - pidVal[1] - pidVal[2];//sqrt(throttle*throttle - pidVal[0]);
				mFR = throttle - pidVal[0] + pidVal[1] + pidVal[2];
				mBL = throttle + pidVal[0] - pidVal[1] + pidVal[2];
				mBR = throttle + pidVal[0] + pidVal[1] - pidVal[2];
				
				mFL = constrain(mFL, 0, 40);
				mFR = constrain(mFR, 0, 40);
				mBL = constrain(mBL, 0, 40);
				mBR = constrain(mBR, 0, 40);
				
				motors[FRONT_LEFT]->setPercent(mFL);
				motors[FRONT_RIGHT]->setPercent(mFR);
				motors[BACK_LEFT]->setPercent(mBL);
				motors[BACK_RIGHT]->setPercent(mBR);
			}else{
				iRot[0] = 0;
				iRot[1] = 0;
				iRot[2] = 0;
				iAng[0] = 0;
				iAng[1] = 0;
				iAng[2] = 0;
				P[0] = 0; I[0] = 0; D[0] = 0;
				P[1] = 0; I[1] = 0; D[1] = 0;
				P[2] = 0; I[2] = 0; D[2] = 0;
				mFL = 0;
				mFR = 0;
				mBL = 0;
				mBR = 0;
			}
			//printf("\t%.2lf\t%.2lf\n", (ang[0]-lAng[0])/interval - fRot[0], (ang[1]-lAng[1])/interval - fRot[1]);
			//printf("\t%.2lf\t%.2lf\t%.2lf\n", gVec[0], gVec[1], gVec[2]);
			lRot[0] = fRot[0]; lRot[1] = fRot[1]; lRot[2] = fRot[2];
			lAng[0] = ang[0]; lAng[1] = ang[1]; lAng[2] = ang[2];
			
			if (counter % 10 == 0) {
				updateRCData();
			}
			
			// Send Log
			if (logLevel && counter % logSendPeriod == 0) {
				log.logLevel = logLevel;
				if (1 == logSendPeriod)
					log.fullInterval = interval;
				else
					log.fullInterval = ((uint64)(currTS.tv_sec-lastLogTS.tv_sec)*1000000000 + currTS.tv_nsec-lastLogTS.tv_nsec - gettimeDelayNs)/1000000000.0;
				lastLogTS = currTS;
				log.armed = armed;
				log.rcThrottle = throttle;
				log.rcPitch = pitch;
				log.rcRoll = roll;
				log.rcYaw = yaw;
				log.pitch = ang[0];
				log.roll = ang[1];
				log.yaw = ang[2];
				
				if (1 == logLevel) {
					conn->send(&log, sizeof(LogMsg1), debugAddress);
				}else{
					log.lastInterval = interval;
					log.motorFL = mFL;
					log.motorFR = mFR;
					log.motorBL = mBL;
					log.motorBR = mBR;
					for (int i=0; i<3; i++) {
						log.P[i] = P[i];
						log.I[i] = I[i];
						log.D[i] = D[i];
					}
					
					if (2 == logLevel) {
						conn->send(&log, sizeof(LogMsg2), debugAddress);
					}else{
						log.fAccel[0] = fAcc[0];
						log.fAccel[1] = fAcc[1];
						log.fAccel[2] = fAcc[2];
						log.fGyro[0] = fRot[0];
						log.fGyro[1] = fRot[1];
						log.fGyro[2] = fRot[2];
						
						if (3 == logLevel) {
							conn->send(&log, sizeof(LogMsg3), debugAddress);
						}else{
							log.accel[0] = acc[0];
							log.accel[1] = acc[1];
							log.accel[2] = acc[2];
							log.gyro[0] = rot[0];
							log.gyro[1] = rot[1];
							log.gyro[2] = rot[2];
							
							conn->send(&log, sizeof(LogMsg4), debugAddress);
						}
					}
				}
			}
					
			counter++;
		}
		
		disarm();
	}
	
	void terminate() {
		terminated = true;
	}
		
	void log(const char*, ...) {
			
	}
	
	void arm() {
		for (int i=0; i<MOTORS_NUM; i++)
			motors[i]->start();
		armed = true;
	}

	void disarm() {
		armed = false;
		for (int i=0; i<MOTORS_NUM; i++)
			motors[i]->stop();
	}
	
	void updateRCData() {
		char msg[100];
		SocketAddress from;
		if (conn->receive(msg, 100, &from) < 0)
			return;
		switch (((ControlMsg*)msg)->type) {
			
			case MSG_ACTION:
				switch (((MsgAction*)msg)->action) {
					case TERMINATE:
						terminate();
						conn->send("\0TERMINATE OK", 14, from);
						break;
					case ARM:
						if (throttle < 1.0) {
							arm();
							conn->send("\0ARM OK", 8, from);
						}else
							conn->send("\0ARM ERROR", 11, from);
						break;
					case DISARM:
						disarm();
						conn->send("\0DISARM OK", 11, from);
						break;
					case CALIBRATE_GYRO:
						if (!armed) {
							conn->send("\0CALIBRATE_GYRO START", 22, from);
							gyro->calibrateGyroscope();
							conn->send("\0CALIBRATE_GYRO DONE", 21, from);
						}else
							conn->send("\0CALIBRATE_GYRO ERROR", 22, from);
						break;
					case CALIBRATE_ACCEL:
						if (!armed) {
							conn->send("\0CALIBRATE_ACCEL START", 23, from);
							accel->calibrateAccelerometer();
							conn->send("\0CALIBRATE_ACCEL DONE", 22, from);
						}else
							conn->send("\0CALIBRATE_ACCEL ERROR", 23, from);
						break;
				}
				break;
				
			case MSG_CONTROL:
				throttle = ((MsgControl*)msg)->throttle / 10.0;
				throttle = constrain(throttle, 0.0, 40.0);
				pitch = ((MsgControl*)msg)->pitch / 10.0;
				pitch = constrain(pitch, -20.0, 20.0);
				roll = ((MsgControl*)msg)->roll / 10.0;
				roll = constrain(roll, -20.0, 20.0);
				yaw = ((MsgControl*)msg)->yaw / 10.0;
				yaw = constrain(yaw, -20.0, 20.0);
				break;
				
			case MSG_DEBUG:
				logLevel = ((MsgDebug*)msg)->logLevel;
				logSendPeriod = ((MsgDebug*)msg)->sendPeriod;
				if (0 == logSendPeriod)
					logLevel = 0;
				debugAddress = from;
				snprintf(msg+1, 100, "DEBUG (%d, %d) OK", logLevel, logSendPeriod);
				msg[0] = 0;
				conn->send(msg, strlen(msg+1) + 2, from);
				break;
				
			case MSG_PID:
				for (int i=0; i<PID_AXIS_NUMBER; i++) {
					pid[i] = ((MsgPID*)msg)->pid[i];
				}
				snprintf(msg+1, 100, "PID YAW (%f, %f, %f) OK\nPID LEVEL (%f, %f, %f) OK", 
					pid[PID_YAW].P, pid[PID_YAW].I, pid[PID_YAW].D, 
					pid[PID_LEVEL].P, pid[PID_LEVEL].I, pid[PID_LEVEL].D);
				msg[0] = 0;
				conn->send(msg, strlen(msg+1) + 2, from);
				break;
				
			default:;
				// Log error
		}
	}
};

#endif