#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/linux/gps.h"
#include <uapi/asm-generic/errno-base.h>

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("argument error : need file path\n");
		return 0;
	}
	
	int err=0;
	const char *pathname = argv[1];
	struct inode *inode;
	struct gps_location *loc = malloc(sizeof(struct gps_location));

	if ((err = syscall(381, pathname, loc)) < 0) {
		printf("[error] sys_get_gps_location : ");
		if(err == -EACCES)
			printf("Permission denied\n");
		else if(err == -ENODEV)
			printf("The file system is not ext2 file system\n");
		else if(err == -EFAULT)
			printf("Bad address\n"); 
		else
			printf("Unexpected error\n");
		return 0;
	}

	int lat_integer = loc->lat_integer;
	int lat_fractional = loc->lat_fractional;
	int lng_integer = loc->lng_integer;
	int lng_fractional = loc->lng_fractional;
	int accuracy = loc->accuracy;

	double lat = lat_integer + (double) lat_fractional / 1000000;
	double lng = lng_integer + (double) lng_fractional / 1000000;


	printf("latitude : %f\nlongitude : %f\naccuracy(m) : %d\n", 
			lat, lng, accuracy);
	printf("https://www.google.com/maps/@%lf,%lf,11z\n", lat, lng);

	return 0;
}
