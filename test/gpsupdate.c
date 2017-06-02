#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/linux/gps.h"
#define ABS(x) (((x) < 0) ? (-1*(x)) : (x))

int main(int argc, char* argv[]) {

	if (argc != 4) {
		printf("arguments error\n");
		return 0;
	}

	double lat = atof(argv[1]);
	double lng = atof(argv[2]);
	int acc = atoi(argv[3]);

	int lat_integer = (int) lat;
	int lat_fractional = ABS((lat - lat_integer) * 1000000);
	int lng_integer = (int) lng;
	int lng_fractional = ABS((lng - lng_integer) * 1000000);


	struct gps_location *loc = malloc(sizeof(struct gps_location));

	loc->lat_integer = lat_integer;
	loc->lat_fractional = lat_fractional;
	loc->lng_integer = lng_integer;
	loc->lng_fractional = lng_fractional;
	loc->accuracy = acc;


	if (syscall(380, loc) < 0) {
		free(loc);
		printf("system call error\n");
		return 0;
	}

	printf("latitude : %d.%d\n lngitude : %d.%d\n accuracy : %d\n", 
			lat_integer, lat_fractional, lng_integer, lng_fractional, acc);



	free(loc);

	return 0;
}
