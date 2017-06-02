#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/linux/gps.h"

int main(int argc, char* argv[]) {

	if (argc != 4) {
		printf("arguments error\n");
		return 0;
	}

	double lat = atof(argv[1]);
	double lng = atof(argv[2]);
	int acc = atoi(argv[3]);

	if (lat < -90 || lat > 90 || lng < -180 || lng > 180 || acc < 0) {
		printf("arguments value error\n");
		return 0;
	}

	int lat_integer = (int) lat;
	int lat_fractional = (lat - lat_integer) * 1000000;
	int lng_integer = (int) lng;
	int lng_fractional = (lng - lng_integer) * 1000000;


	printf("lat_int : %d  lat_flo : %d\nlng_int : %d	lng_flo : %d\n", 
			lat_integer, lat_fractional, lng_integer, lng_fractional);

	struct gps_location *loc = malloc(sizeof(struct gps_location));

	loc->lat_integer = lat_integer;
	loc->lat_fractional = lat_fractional;
	loc->lng_integer = lng_integer;
	loc->lng_fractional = lng_fractional;
	loc->accuracy = acc;

	syscall(380, loc);

	free(loc);

	return 0;
}
