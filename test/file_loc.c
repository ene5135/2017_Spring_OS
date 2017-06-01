#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/linux/gps.h"

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("no file path error\n");
		return 0;
	}

	const char *pathname = argv[1];
	struct inode *inode;
	struct gps_location *loc;

	if (syscall(381, pathname, loc) < 0) {
		printf("error\n");
		return 0;
	}

	int lat_integer = loc->lat_integer;
	int lat_fractional = loc->lat_fractional;
	int lng_integer = loc->lng_integer;
	int lng_fractional = loc->lng_fractional;
	int accuracy = loc->accuracy;

	double lat = lat_integer + lat_fractional / 1000000;
	double lng = lng_integer + lng_fractional / 1000000;


	printf("latitude : %f\nlongitude : %f\naccuracy(m) : %d\n", 
			lat, lng, accuracy);
	printf("https://www.google.com/maps/@%lf,%lf,11z", lat, lng);

	return 0;
}
