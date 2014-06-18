#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "LocalI2CBus.h"
#include "ADXL345.h"
#include "L3G4200D.h"


void rotateV(double* v, double* delta) {
	double v_tmp[3];
	v_tmp[0] = v[0];
	v_tmp[1] = v[1];
	v_tmp[2] = v[2];

	v[2] += - delta[1]  * v_tmp[0] + delta[0] * v_tmp[1];
	v[0] += delta[1]  * v_tmp[2] - delta[2]   * v_tmp[1];
	v[1] += - delta[0] * v_tmp[2] + delta[2]   * v_tmp[0];
}

int main() {

	LocalI2CBus bus(1);
	
	ADXL345 acc(&bus);
	L3G4200D gyro(&bus);
		
	acc.setEnabled(false);
	acc.setDataRate(ADXL345::DATARATE_100_HZ);
	acc.setRange(ADXL345::RANGE_16G);
	acc.setFullResolutionEnabled(true);
	acc.setEnabled(true);

	gyro.setEnabled(false);
	gyro.setOutputDataRate(L3G4200D::DATARATE_800_HZ);
	//gyro.setBlockDataUpdateEnabled(false);
	gyro.setEnabled(true);

	timespec ts, ts2;
	clock_gettime(CLOCK_REALTIME, &ts);
	for (int i=0; i<1000; i++)
		clock_gettime(CLOCK_REALTIME, &ts2);
	int gettime_delay_ns = ((ts2.tv_sec-ts.tv_sec)*1000000000 + ts2.tv_nsec-ts.tv_nsec) / 1000;
	printf("gettime delay: %d\n", gettime_delay_ns);
	usleep(1000000);
	
	
	double accv[32][3];
	double rotv[100][3];
	double rota[3], rot[3];
	for (int i=0; i<10000; i++) {
		//acc.getLinearAcceleration(accv[i][0], accv[i][1], accv[i][2]);
		//gyro.getAngularVelocity(rotv[i][0], rotv[i][1], rotv[i][2]);
		gyro.getAngularVelocity(rot[0], rot[1], rot[2]);
		rota[0] += rot[0];
		rota[1] += rot[1];
		rota[2] += rot[2];
		clock_gettime(CLOCK_REALTIME, &ts2);
		//usleep(10000);
	}
	rota[0] /= 10000;
	rota[1] /= 10000;
	rota[2] /= 10000;
	/*
	for (int i=0; i<100; i++) {
		//printf("\t%.2lf\t%.2lf\t%.2lf\n", accv[i][0], accv[i][1], accv[i][2]);
		//printf("\t%.2lf\t%.2lf\t%.2lf\n", rotv[i][0], rotv[i][1], rotv[i][2]);
	}
	printf("\n\n");
	printf("\t%.2lf\t%.2lf\t%.2lf\n", rota[0]/32, rota[1]/32, rota[2]/32);
	printf("\n\n");
	usleep(10000000);
	*/
	
	acc.calibrate(100);

	int8 ox, oy, oz;
	acc.getOffset(ox, oy, oz);
	printf("%d\t%d\t%d\n", ox, oy, oz);
	usleep(1000000);

	int16 x, y, z;
	double acc_x = 0, acc_y = 0, acc_z = 0, mag;
	double lacc_x, lacc_y, lacc_z;
	double rot_x, rot_y, rot_z;
	double ang_x = 0, ang_y = 0, ang_z = 0, interval, dang[3], gvec[3] = {0, 0, 0};
	
	clock_gettime(CLOCK_REALTIME, &ts);
		
	while (true) {
		acc.getLinearAcceleration(lacc_x, lacc_y, lacc_z);
		gyro.getAngularVelocity(rot_x, rot_y, rot_z);
		acc_x = (15 * acc_x + lacc_x) / 16;
		acc_y = (15 * acc_y + lacc_y) / 16;
		acc_z = (15 * acc_z + lacc_z) / 16;
		mag = acc_x*acc_x+acc_y*acc_y+acc_z*acc_z;
		//printf("\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t|\t%.2lf\t%.2lf\t%.2lf", acc_x, acc_y, acc_z, mag, rot_x, rot_y, rot_z);
		//usleep(10000);
		clock_gettime(CLOCK_REALTIME, &ts2);
		interval = ((ts2.tv_sec-ts.tv_sec)*1000000000 + ts2.tv_nsec-ts.tv_nsec - gettime_delay_ns)/1000.0;
		ts = ts2;
		dang[0] = - (rot_x - rota[0]) * interval / 1000000;
		dang[1] = - (rot_y - rota[1]) * interval / 1000000;
		dang[2] = - (rot_z - rota[2]) * interval / 1000000;
		ang_x += dang[0];
		ang_y += dang[1];
		ang_z += dang[2];
		dang[0] *= 3.14 / 180;
		dang[1] *= 3.14 / 180;
		dang[2] *= 3.14 / 180;
		rotateV(gvec, dang);
		mag = mag * 100 / (9.81 * 9.81);
		if (72.0 < mag && mag < 133.0) {
			gvec[0] = (gvec[0] * 600 + acc_x) / 601;
			gvec[1] = (gvec[1] * 600 + acc_y) / 601;
			gvec[2] = (gvec[2] * 600 + acc_z) / 601;
		}
		printf("\t%.2lf\t%.2lf\t%.2lf\t|\t%.2lf\t%.2lf\t|\t%.2lf\t%.2lf\t%.2lf\t[%.2lf]\n", ang_x, ang_y, ang_z, 
			atan2(gvec[1], sqrt(gvec[0]*gvec[0] + gvec[2]*gvec[2])) * 180/ 3.14, 
			atan2(gvec[0], gvec[2]) * 180/ 3.14, gvec[0], gvec[1], gvec[2], interval);
	}

}