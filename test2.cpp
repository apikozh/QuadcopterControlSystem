#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int main() {

	timespec ts, ts2;
	clock_gettime(CLOCK_REALTIME, &ts);
	
	/*double tmp = 2343.464;
	for (int i=0; i<10000; i++) {
		tmp += (cos(tmp*tan(tmp+ 12345))/(tmp+12343.46645) + sin(23242/(tmp+1455.)))/(cos(tmp*tan(tmp+ 12345))/(tmp+12343.46645) + sin(23242/(tmp+1455.)));
	}*/
	usleep(500);
	
	clock_gettime(CLOCK_REALTIME, &ts2);
	int gettime_delay_ns = (ts2.tv_sec-ts.tv_sec)*1000000000 + ts2.tv_nsec-ts.tv_nsec;
	printf("interval: %d\n", gettime_delay_ns);
	//printf("val: %lf\n", tmp);
	
}